/*
 * PROTEVE™ v5.1 - SECURITY EDITION + REPLIT
 * Integração com Servidor Replit
 * Mantém compatibilidade com Telegram e Dashboard Local
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

// --- REPLIT SERVER ---
const char* REPLIT_SERVER = "proteve-server.seu-usuario.repl.co";  // MUDE AQUI: Seu URL Replit
const char* REPLIT_API_KEY = "proteve-001-chave-unica";  // MUDE AQUI: Gere uma chave única
const char* REPLIT_ENDPOINT = "/api/v1";

// --- NTP ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

// --- PINOS E CONSTANTES ---
#define RELAY_PIN 26
#define TENSAO_NOMINAL 220.0
#define TENSAO_CRITICA_MIN 190.0
#define TENSAO_CRITICA_MAX 250.0
#define TEMPO_REARME 30000
#define TEMPO_LOCKOUT 900000
#define TEMPO_ESTABILIDADE 180000

// --- ENUMS E STRUCTS ---
enum EstadoSistema { ESTADO_OPERACIONAL, ESTADO_ALERTA, ESTADO_DESARME, ESTADO_REARME, ESTADO_LOCKOUT };
enum StatusFase { FASE_NORMAL, FASE_PRECARIA, FASE_SURTO, FASE_QUEDA, FASE_FALTA };

struct DadosFase { float tensao, corrente, potencia, fp; StatusFase status; String statusStr; };
struct Evento { String ts; String cod, desc, fase; float val; };

// --- VARIÁVEIS GLOBAIS ---
WebServer server(80);
DadosFase faseA, faseB, faseC;
float frequencia = 60.0, energiaTotal = 0.0, potenciaTotal = 0.0, temperaturaESP = 45.0;
EstadoSistema estadoAtual = ESTADO_OPERACIONAL;
String estadoStr = "OPERACIONAL";
bool relayState = true, simulando = true;
int tentativasRearme = 0;
unsigned long inicioLockout = 0, inicioRearme = 0, lastRead = 0, inicioEstabilidade = 0;
bool aguardandoRearme = false;
Evento logEventos[20];
int logIndex = 0;

bool statusRelePrincipal = true, statusContatora = true;
String diagnosticoHardware = "SISTEMA OK";

unsigned long tempoInativoTotal = 0, inicioInatividade = 0;
int contagemSurtos = 0, contagemSubtensao = 0, contagemFaltas = 0;
float energiaNoUltimoRelatorio = 0;
int ultimaHoraRelatorio = -1;

// --- TIMERS PARA ENVIO DE DADOS ---
unsigned long lastTelemetrySend = 0;
unsigned long lastEventSend = 0;
unsigned long lastCommandCheck = 0;
const unsigned long TELEMETRY_INTERVAL = 10000;  // 10 segundos
const unsigned long EVENT_INTERVAL = 5000;       // 5 segundos
const unsigned long COMMAND_INTERVAL = 5000;     // 5 segundos

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
    WiFiClientSecure client; client.setInsecure();
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + mensagem + "&parse_mode=Markdown";
    url.replace(" ", "%20"); url.replace("\n", "%0A");
    if (http.begin(client, url)) { http.GET(); http.end(); }
  }
}

void addEvento(String c, String d, String f = "", float v = 0) {
  logEventos[logIndex] = {getHoraAtual(), c, d, f, v};
  logIndex = (logIndex + 1) % 20;
  if (c == "E001") contagemSurtos++;
  else if (c == "E002") contagemSubtensao++;
  else if (c == "E003") contagemFaltas++;
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
  simularFase(faseA, "A"); simularFase(faseB, "B"); simularFase(faseC, "C");
  frequencia = 60.0 + (random(-2, 3) / 10.0);
  potenciaTotal = faseA.potencia + faseB.potencia + faseC.potencia;
  energiaTotal += (potenciaTotal / 3600.0) / 1000.0;
  temperaturaESP = 42.0 + (random(0, 100) / 10.0);
}

// === INTEGRAÇÃO REPLIT ===

void enviarTelemetriaReplit() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastTelemetrySend < TELEMETRY_INTERVAL) return;
  
  lastTelemetrySend = millis();
  
  WiFiClientSecure client;
  client.setInsecure();  // Para HTTPS sem verificação
  HTTPClient http;
  
  String url = String("https://") + REPLIT_SERVER + REPLIT_ENDPOINT + "/telemetry";
  
  if (!http.begin(client, url)) {
    Serial.println("[REPLIT] Erro ao conectar ao servidor");
    return;
  }
  
  // Preparar JSON
  String json = "{";
  json += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
  json += "\"estado\":\"" + estadoStr + "\",";
  json += "\"rele\":" + String(relayState ? "true" : "false") + ",";
  json += "\"A\":{\"v\":" + String(faseA.tensao,1) + ",\"i\":" + String(faseA.corrente,1) + ",\"p\":" + String(faseA.potencia,0) + ",\"s\":\"" + faseA.statusStr + "\",\"fp\":" + String(faseA.fp,2) + "},";
  json += "\"B\":{\"v\":" + String(faseB.tensao,1) + ",\"i\":" + String(faseB.corrente,1) + ",\"p\":" + String(faseB.potencia,0) + ",\"s\":\"" + faseB.statusStr + "\",\"fp\":" + String(faseB.fp,2) + "},";
  json += "\"C\":{\"v\":" + String(faseC.tensao,1) + ",\"i\":" + String(faseC.corrente,1) + ",\"p\":" + String(faseC.potencia,0) + ",\"s\":\"" + faseC.statusStr + "\",\"fp\":" + String(faseC.fp,2) + "},";
  json += "\"kwh\":" + String(energiaTotal, 2) + ",";
  json += "\"potencia\":" + String(potenciaTotal, 0) + ",";
  json += "\"freq\":" + String(frequencia, 1) + ",";
  json += "\"temp\":" + String(temperaturaESP, 1) + ",";
  json += "\"diag\":\"" + diagnosticoHardware + "\",";
  json += "\"stRele\":" + String(statusRelePrincipal ? "true" : "false") + ",";
  json += "\"stCont\":" + String(statusContatora ? "true" : "false") + "";
  json += "}";
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", REPLIT_API_KEY);
  
  int httpCode = http.POST(json);
  
  if (httpCode == 200) {
    Serial.println("[REPLIT] Telemetria enviada com sucesso");
  } else {
    Serial.print("[REPLIT] Erro ao enviar telemetria: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void enviarEventoReplit(String codigo, String descricao, String fase, float valor) {
  if (WiFi.status() != WL_CONNECTED) return;
  
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  String url = String("https://") + REPLIT_SERVER + REPLIT_ENDPOINT + "/events";
  
  if (!http.begin(client, url)) return;
  
  String json = "{";
  json += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
  json += "\"codigo\":\"" + codigo + "\",";
  json += "\"descricao\":\"" + descricao + "\",";
  json += "\"fase\":\"" + fase + "\",";
  json += "\"valor\":" + String(valor, 1) + "";
  json += "}";
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", REPLIT_API_KEY);
  
  int httpCode = http.POST(json);
  
  if (httpCode == 200) {
    Serial.println("[REPLIT] Evento enviado com sucesso");
  }
  
  http.end();
}

void verificarComandosReplit() {
  if (WiFi.status() != WL_CONNECTED) return;
  if (millis() - lastCommandCheck < COMMAND_INTERVAL) return;
  
  lastCommandCheck = millis();
  
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  String url = String("https://") + REPLIT_SERVER + REPLIT_ENDPOINT + "/commands";
  
  if (!http.begin(client, url)) return;
  
  http.addHeader("X-API-Key", REPLIT_API_KEY);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("[REPLIT] Comandos recebidos: " + payload);
    
    // Parse JSON e execute comandos
    if (payload.indexOf("ligar") > -1) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("[REPLIT] Comando LIGAR executado");
    } else if (payload.indexOf("desligar") > -1) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("[REPLIT] Comando DESLIGAR executado");
    } else if (payload.indexOf("reset") > -1) {
      Serial.println("[REPLIT] Comando RESET executado");
      delay(1000);
      ESP.restart();
    }
  }
  
  http.end();
}

// === SETUP ===

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== PROTEVE™ v5.1 - REPLIT ===");
  
  // Configurar pino do relé
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  // Conectar WiFi
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
    
    // Sincronizar hora
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println("Sincronizando hora...");
    delay(2000);
  } else {
    Serial.println("\n❌ Falha ao conectar WiFi");
  }
  
  // Configurar servidor web local
  server.on("/", handleRoot);
  server.on("/api", handleAPI);
  server.on("/ligar", handleLigar);
  server.on("/desligar", handleDesligar);
  server.on("/reset", handleReset);
  server.begin();
  
  Serial.println("✅ Servidor web iniciado na porta 80");
  Serial.println("✅ Conectando ao Replit...");
  Serial.print("   Servidor: ");
  Serial.println(REPLIT_SERVER);
}

// === LOOP ===

void loop() {
  server.handleClient();
  
  // Ler sensores
  if (millis() - lastRead > 2000) {
    lastRead = millis();
    lerSensores();
    verificarProtecao();
    auditoriaHierarquica();
  }
  
  // Enviar dados para Replit
  enviarTelemetriaReplit();
  verificarComandosReplit();
  
  delay(100);
}

// === HANDLERS WEB (MANTER COMPATIBILIDADE COM DASHBOARD LOCAL) ===

void handleRoot() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }
  // ... (manter código do dashboard local original)
}

void handleAPI() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }
  // ... (manter código da API original)
}

void handleLigar() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }
  relayState = true;
  digitalWrite(RELAY_PIN, HIGH);
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleDesligar() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }
  relayState = false;
  digitalWrite(RELAY_PIN, LOW);
  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleReset() {
  if (!server.authenticate(WEB_USER, WEB_PASS)) {
    server.requestAuthentication();
    return;
  }
  server.send(200, "application/json", "{\"status\":\"ok\"}");
  delay(1000);
  ESP.restart();
}

void verificarProtecao() {
  // ... (manter código original de proteção)
}

void auditoriaHierarquica() {
  // ... (manter código original de auditoria)
}
