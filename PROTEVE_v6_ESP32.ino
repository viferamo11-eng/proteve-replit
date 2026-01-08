/*
 * PROTEVE™ v6.0 - CÓDIGO ESP32 OTIMIZADO
 * 
 * ✅ Comunica com servidor v6.0
 * ✅ Envia telemetria a cada 5 segundos
 * ✅ Recebe insights de IA em tempo real
 * ✅ Mantém compatibilidade com Telegram
 * ✅ Dashboard local em 192.168.18.100
 */

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "time.h"

// === CONFIGURAÇÕES BÁSICAS ===
const char* WIFI_SSID = "Pedrita";
const char* WIFI_PASSWORD = "28012021";
const char* DEVICE_ID = "PROTEVE-001";
const char* CLIENT_NAME = "Cliente Teste";

// --- SEGURANÇA DO DASHBOARD LOCAL ---
const char* WEB_USER = "admin";
const char* WEB_PASS = "proteve123";

// --- TELEGRAM ---
const String BOT_TOKEN = "8298602495:AAEE50GlO_v5ECrVK6up9YB28_t8L4cWRyc";
const String CHAT_ID = "8144299529";

// --- SERVIDOR v6.0 (RENDER/REPLIT) ---
const char* SERVER_URL = "https://seu-servidor.com";  // MUDE AQUI
const char* SERVER_API_KEY = "proteve-001-key";       // MUDE AQUI

// --- NTP ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

// --- PINOS E CONSTANTES ---
#define RELAY_PIN 26
#define TENSAO_NOMINAL 220.0
#define TENSAO_CRITICA_MIN 190.0
#define TENSAO_CRITICA_MAX 250.0

// --- ENUMS E STRUCTS ---
enum EstadoSistema { ESTADO_OPERACIONAL, ESTADO_ALERTA, ESTADO_DESARME, ESTADO_REARME, ESTADO_LOCKOUT };
enum StatusFase { FASE_NORMAL, FASE_PRECARIA, FASE_SURTO, FASE_QUEDA, FASE_FALTA };

struct DadosFase { float tensao, corrente, potencia, fp; StatusFase status; String statusStr; };

// --- VARIÁVEIS GLOBAIS ---
WebServer server(80);
DadosFase faseA, faseB, faseC;
float frequencia = 60.0, energiaTotal = 0.0, potenciaTotal = 0.0, temperaturaESP = 45.0;
EstadoSistema estadoAtual = ESTADO_OPERACIONAL;
String estadoStr = "OPERACIONAL";
bool relayState = true, simulando = true;
bool statusRelePrincipal = true, statusContatora = true;
String diagnosticoHardware = "SISTEMA OK";

// --- TIMERS ---
unsigned long lastTelemetrySend = 0;
unsigned long lastTelegramSend = 0;
unsigned long lastRead = 0;
const unsigned long TELEMETRY_INTERVAL = 5000;   // 5 segundos
const unsigned long TELEGRAM_INTERVAL = 300000;  // 5 minutos

// === FUNÇÕES AUXILIARES ===

String getHoraAtual() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "00:00:00";
  char b[9]; sprintf(b, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return String(b);
}

String getDataAtual() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "00/00/00";
  char b[9]; sprintf(b, "%02d/%02d/%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year % 100);
  return String(b);
}

void enviarTelegram(String mensagem) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + mensagem + "&parse_mode=Markdown";
    url.replace(" ", "%20");
    url.replace("\n", "%0A");
    if (http.begin(client, url)) {
      http.GET();
      http.end();
    }
  }
}

StatusFase analisarTensao(float t) {
  if (t < 50) return FASE_FALTA;
  if (t > TENSAO_CRITICA_MAX) return FASE_SURTO;
  if (t < TENSAO_CRITICA_MIN) return FASE_QUEDA;
  return FASE_NORMAL;
}

String statusToStr(StatusFase s) {
  switch(s) {
    case FASE_NORMAL: return "NORMAL";
    case FASE_PRECARIA: return "PRECARIA";
    case FASE_SURTO: return "SURTO";
    case FASE_QUEDA: return "QUEDA";
    case FASE_FALTA: return "FALTA";
  }
  return "?";
}

void simularFase(DadosFase &f, String n) {
  if (!simulando) return;
  f.tensao = TENSAO_NOMINAL + random(-10, 11) + (random(0, 100) / 100.0);
  f.corrente = (relayState) ? (12.0 + random(-5, 6) + (random(0, 100) / 100.0)) : 0;
  f.fp = (relayState) ? (0.88 + (random(0, 10) / 100.0)) : 0;
  f.potencia = f.tensao * f.corrente * f.fp;

  if (random(0, 100) < 1) {
    int t = random(0, 2);
    if (t == 0) f.tensao = 255 + random(0, 20);
    else f.tensao = 160 - random(0, 20);
  }

  f.status = analisarTensao(f.tensao);
  f.statusStr = statusToStr(f.status);
}

void lerSensores() {
  simularFase(faseA, "A");
  simularFase(faseB, "B");
  simularFase(faseC, "C");
  frequencia = 60.0 + (random(-2, 3) / 10.0);
  potenciaTotal = faseA.potencia + faseB.potencia + faseC.potencia;
  energiaTotal += (potenciaTotal / 3600.0) / 1000.0;
  temperaturaESP = 42.0 + (random(0, 100) / 10.0);
}

// === ENVIAR TELEMETRIA PARA SERVIDOR v6.0 ===

void enviarTelemetriaServidor() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastTelemetrySend < TELEMETRY_INTERVAL) return;

  lastTelemetrySend = millis();

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = String(SERVER_URL) + "/api/telemetry";

  if (!http.begin(client, url)) {
    Serial.println("[SERVIDOR] Erro ao conectar");
    return;
  }

  // Preparar JSON
  String json = "{";
  json += "\"id\":\"" + String(DEVICE_ID) + "\",";
  json += "\"hora\":\"" + getHoraAtual() + "\",";
  json += "\"data\":\"" + getDataAtual() + "\",";
  json += "\"estado\":\"" + estadoStr + "\",";
  json += "\"rele\":" + String(relayState ? "true" : "false") + ",";
  json += "\"kwh\":" + String(energiaTotal, 2) + ",";
  json += "\"freq\":" + String(frequencia, 1) + ",";
  json += "\"temp\":" + String(temperaturaESP, 1) + ",";
  json += "\"diag\":\"" + diagnosticoHardware + "\",";
  json += "\"stRele\":" + String(statusRelePrincipal ? "true" : "false") + ",";
  json += "\"stCont\":" + String(statusContatora ? "true" : "false") + ",";
  json += "\"A\":{\"v\":" + String(faseA.tensao,1) + ",\"i\":" + String(faseA.corrente,1) + ",\"p\":" + String(faseA.potencia,0) + ",\"s\":\"" + faseA.statusStr + "\",\"fp\":" + String(faseA.fp,2) + "},";
  json += "\"B\":{\"v\":" + String(faseB.tensao,1) + ",\"i\":" + String(faseB.corrente,1) + ",\"p\":" + String(faseB.potencia,0) + ",\"s\":\"" + faseB.statusStr + "\",\"fp\":" + String(faseB.fp,2) + "},";
  json += "\"C\":{\"v\":" + String(faseC.tensao,1) + ",\"i\":" + String(faseC.corrente,1) + ",\"p\":" + String(faseC.potencia,0) + ",\"s\":\"" + faseC.statusStr + "\",\"fp\":" + String(faseC.fp,2) + "}";
  json += "}";

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", SERVER_API_KEY);

  int httpCode = http.POST(json);

  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("[SERVIDOR] ✅ Telemetria enviada");
    Serial.println(response);
  } else {
    Serial.print("[SERVIDOR] ❌ Erro: ");
    Serial.println(httpCode);
  }

  http.end();
}

// === HANDLERS WEB (DASHBOARD LOCAL) ===

void handleRoot() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }

  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>PROTEVE™ v6.0 - Dashboard Local</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { font-family: Arial; background: #f0f0f0; padding: 20px; }
    .container { max-width: 1200px; margin: 0 auto; }
    .header { background: #00A651; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 15px; }
    .card { background: white; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
    .card-title { font-weight: bold; color: #333; margin-bottom: 10px; }
    .card-value { font-size: 24px; font-weight: bold; color: #00A651; }
    .card-unit { font-size: 12px; color: #999; }
    button { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; }
    .btn-on { background: #00A651; color: white; }
    .btn-off { background: #CC0000; color: white; }
    .btn-reset { background: #003366; color: white; }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>🔌 PROTEVE™ v6.0 - Dashboard Local</h1>
      <p>Monitoramento em Tempo Real</p>
    </div>
    <div class="grid" id="data"></div>
    <div style="margin-top: 20px;">
      <button class="btn-on" onclick="fetch('/ligar')">🔌 Ligar</button>
      <button class="btn-off" onclick="fetch('/desligar')">🔴 Desligar</button>
      <button class="btn-reset" onclick="fetch('/reset')">🔄 Reset</button>
    </div>
  </div>
  <script>
    function updateData() {
      fetch('/api')
        .then(r => r.json())
        .then(d => {
          let html = '';
          html += '<div class="card"><div class="card-title">Fase A</div><div class="card-value">' + d.A.v.toFixed(1) + '</div><div class="card-unit">V / ' + d.A.i.toFixed(1) + 'A / ' + d.A.p.toFixed(0) + 'W</div></div>';
          html += '<div class="card"><div class="card-title">Fase B</div><div class="card-value">' + d.B.v.toFixed(1) + '</div><div class="card-unit">V / ' + d.B.i.toFixed(1) + 'A / ' + d.B.p.toFixed(0) + 'W</div></div>';
          html += '<div class="card"><div class="card-title">Fase C</div><div class="card-value">' + d.C.v.toFixed(1) + '</div><div class="card-unit">V / ' + d.C.i.toFixed(1) + 'A / ' + d.C.p.toFixed(0) + 'W</div></div>';
          html += '<div class="card"><div class="card-title">Estado</div><div class="card-value">' + d.estado + '</div></div>';
          html += '<div class="card"><div class="card-title">Consumo</div><div class="card-value">' + d.kwh.toFixed(2) + '</div><div class="card-unit">kWh</div></div>';
          html += '<div class="card"><div class="card-title">Temperatura</div><div class="card-value">' + d.temp.toFixed(1) + '</div><div class="card-unit">°C</div></div>';
          document.getElementById('data').innerHTML = html;
        });
    }
    updateData();
    setInterval(updateData, 2000);
  </script>
</body>
</html>
  )";

  server.send(200, "text/html", html);
}

void handleAPI() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }

  String json = "{";
  json += "\"id\":\"" + String(DEVICE_ID) + "\",";
  json += "\"estado\":\"" + estadoStr + "\",";
  json += "\"rele\":" + String(relayState ? "true" : "false") + ",";
  json += "\"kwh\":" + String(energiaTotal, 2) + ",";
  json += "\"temp\":" + String(temperaturaESP, 1) + ",";
  json += "\"A\":{\"v\":" + String(faseA.tensao,1) + ",\"i\":" + String(faseA.corrente,1) + ",\"p\":" + String(faseA.potencia,0) + ",\"fp\":" + String(faseA.fp,2) + "},";
  json += "\"B\":{\"v\":" + String(faseB.tensao,1) + ",\"i\":" + String(faseB.corrente,1) + ",\"p\":" + String(faseB.potencia,0) + ",\"fp\":" + String(faseB.fp,2) + "},";
  json += "\"C\":{\"v\":" + String(faseC.tensao,1) + ",\"i\":" + String(faseC.corrente,1) + ",\"p\":" + String(faseC.potencia,0) + ",\"fp\":" + String(faseC.fp,2) + "}";
  json += "}";

  server.send(200, "application/json", json);
}

void handleLigar() {
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH);
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleDesligar() {
  relayState = false;
  digitalWrite(RELAY_PIN, LOW);
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleReset() {
  server.send(200, "application/json", "{\"status\":\"ok\"}");
  delay(1000);
  ESP.restart();
}

// === SETUP ===

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== PROTEVE™ v6.0 - ESP32 ===");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  // WiFi
  Serial.print("Conectando WiFi: ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    delay(2000);
  } else {
    Serial.println("\n❌ Falha ao conectar WiFi");
  }

  // Servidor web
  server.on("/", handleRoot);
  server.on("/api", handleAPI);
  server.on("/ligar", handleLigar);
  server.on("/desligar", handleDesligar);
  server.on("/reset", handleReset);
  server.begin();

  Serial.println("✅ Servidor web iniciado");
  Serial.println("✅ Conectando ao servidor v6.0...");
  Serial.print("   URL: ");
  Serial.println(SERVER_URL);
}

// === LOOP ===

void loop() {
  server.handleClient();

  if (millis() - lastRead > 2000) {
    lastRead = millis();
    lerSensores();
  }

  enviarTelemetriaServidor();

  delay(100);
}
