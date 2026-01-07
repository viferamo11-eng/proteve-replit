# 🚀 PROTEVE™ Replit - Setup em 2 Minutos!

## ✅ Tudo Pronto para Você!

Seu repositório GitHub já está criado com todos os arquivos:
**https://github.com/viferamo11-eng/proteve-replit**

---

## 🎯 Passo 1: Ir para Replit

1. Abra: **https://replit.com**
2. Clique em **"Sign up"** ou **"Log in"**
3. Escolha **"Sign in with GitHub"**
4. Autorize o Replit a acessar seu GitHub

---

## 🎯 Passo 2: Importar Repositório

1. Após fazer login, clique em **"+ Create"**
2. Escolha **"Import from GitHub"**
3. Cole este link:
   ```
   https://github.com/viferamo11-eng/proteve-replit
   ```
4. Clique em **"Import"**
5. Aguarde carregar (30 segundos)

---

## 🎯 Passo 3: Criar Pasta Templates

1. No Replit, clique no ícone de **pasta** (lado esquerdo)
2. Clique em **"New Folder"**
3. Nome: `templates`
4. Pressione Enter

---

## 🎯 Passo 4: Criar Dashboard HTML

1. Dentro da pasta `templates`, clique em **"New File"**
2. Nome: `index.html`
3. Cole este código:

```html
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PROTEVE™ Dashboard</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; padding: 20px; }
        .container { max-width: 1200px; margin: 0 auto; }
        header { background: rgba(255, 255, 255, 0.95); padding: 20px; border-radius: 10px; margin-bottom: 30px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); }
        h1 { color: #333; margin-bottom: 10px; }
        .subtitle { color: #666; font-size: 14px; }
        .devices-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .device-card { background: rgba(255, 255, 255, 0.95); border-radius: 10px; padding: 20px; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); transition: transform 0.3s ease; }
        .device-card:hover { transform: translateY(-5px); }
        .device-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; border-bottom: 2px solid #eee; padding-bottom: 10px; }
        .device-name { font-size: 18px; font-weight: bold; color: #333; }
        .status-badge { padding: 5px 10px; border-radius: 20px; font-size: 12px; font-weight: bold; }
        .status-online { background: #4caf50; color: white; }
        .status-offline { background: #f44336; color: white; }
        .device-info { margin-bottom: 15px; }
        .info-row { display: flex; justify-content: space-between; padding: 8px 0; border-bottom: 1px solid #f0f0f0; font-size: 14px; }
        .info-label { color: #666; font-weight: 500; }
        .info-value { color: #333; font-weight: bold; }
        .device-controls { display: flex; gap: 10px; margin-top: 15px; }
        button { flex: 1; padding: 10px; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; transition: background 0.3s ease; }
        .btn-on { background: #4caf50; color: white; }
        .btn-on:hover { background: #45a049; }
        .btn-off { background: #f44336; color: white; }
        .btn-off:hover { background: #da190b; }
        .btn-reset { background: #2196F3; color: white; }
        .btn-reset:hover { background: #0b7dda; }
        .loading { text-align: center; color: white; font-size: 18px; padding: 40px; }
        .error { background: #f44336; color: white; padding: 15px; border-radius: 5px; margin-bottom: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🔌 PROTEVE™ Dashboard</h1>
            <p class="subtitle">Gerenciador de Dispositivos Remoto</p>
        </header>
        
        <div id="error" class="error" style="display: none;"></div>
        <div id="loading" class="loading">Carregando dispositivos...</div>
        <div id="devices" class="devices-grid"></div>
    </div>
    
    <script>
        async function loadDevices() {
            try {
                const response = await fetch('/api/devices');
                const devices = await response.json();
                
                document.getElementById('loading').style.display = 'none';
                
                if (devices.length === 0) {
                    document.getElementById('devices').innerHTML = '<p style="color: white;">Nenhum dispositivo registrado</p>';
                    return;
                }
                
                let html = '';
                devices.forEach(device => {
                    const status = device.is_online ? 'online' : 'offline';
                    const statusText = device.is_online ? '🟢 Online' : '🔴 Offline';
                    const lastSeen = device.last_seen ? new Date(device.last_seen).toLocaleString('pt-BR') : 'Nunca';
                    
                    const telemetry = device.latest_telemetry || {};
                    
                    html += `
                        <div class="device-card">
                            <div class="device-header">
                                <div class="device-name">${device.name}</div>
                                <div class="status-badge status-${status}">${statusText}</div>
                            </div>
                            
                            <div class="device-info">
                                <div class="info-row">
                                    <span class="info-label">ID:</span>
                                    <span class="info-value">${device.device_id}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Cliente:</span>
                                    <span class="info-value">${device.client_name || 'N/A'}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Estado:</span>
                                    <span class="info-value">${telemetry.estado || 'N/A'}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Relé:</span>
                                    <span class="info-value">${telemetry.rele ? '✅ ON' : '❌ OFF'}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Temperatura:</span>
                                    <span class="info-value">${telemetry.temperatura ? telemetry.temperatura.toFixed(1) + '°C' : 'N/A'}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Potência:</span>
                                    <span class="info-value">${telemetry.potencia_total ? telemetry.potencia_total.toFixed(0) + 'W' : 'N/A'}</span>
                                </div>
                                <div class="info-row">
                                    <span class="info-label">Última atualização:</span>
                                    <span class="info-value" style="font-size: 12px;">${lastSeen}</span>
                                </div>
                            </div>
                            
                            <div class="device-controls">
                                <button class="btn-on" onclick="sendCommand(${device.id}, 'ligar')">🔌 Ligar</button>
                                <button class="btn-off" onclick="sendCommand(${device.id}, 'desligar')">🔴 Desligar</button>
                                <button class="btn-reset" onclick="sendCommand(${device.id}, 'reset')">🔄 Reset</button>
                            </div>
                        </div>
                    `;
                });
                
                document.getElementById('devices').innerHTML = html;
            } catch (error) {
                console.error('Erro ao carregar dispositivos:', error);
                document.getElementById('error').textContent = 'Erro ao carregar dispositivos: ' + error.message;
                document.getElementById('error').style.display = 'block';
            }
        }
        
        async function sendCommand(deviceId, comando) {
            try {
                const response = await fetch(`/api/devices/${deviceId}/command`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify({ comando: comando })
                });
                
                if (response.ok) {
                    alert(`Comando "${comando}" enviado com sucesso!`);
                    loadDevices();
                } else {
                    alert('Erro ao enviar comando');
                }
            } catch (error) {
                console.error('Erro:', error);
                alert('Erro ao enviar comando: ' + error.message);
            }
        }
        
        loadDevices();
        setInterval(loadDevices, 5000);
    </script>
</body>
</html>
```

---

## 🎯 Passo 5: Instalar Dependências

1. Clique na aba **"Shell"** (lado direito)
2. Digite:
   ```bash
   pip install -r requirements_replit.txt
   ```
3. Pressione Enter
4. Aguarde terminar

---

## 🎯 Passo 6: Rodar o Servidor

1. Clique no botão **"Run"** (topo)
2. Aguarde iniciar
3. Você verá um link como:
   ```
   https://proteve-replit.seu-usuario.repl.co
   ```

---

## 🎯 Passo 7: Acessar o Dashboard

1. Clique no link que apareceu
2. Você verá o dashboard!
3. Salve este link!

---

## 🎯 Passo 8: Configurar ESP32

No código `PROTEVE_v5_1_REPLIT.ino`, mude estas linhas:

```cpp
// Linha ~35
const char* REPLIT_SERVER = "proteve-replit.seu-usuario.repl.co";
const char* REPLIT_API_KEY = "proteve-001-chave-unica";
```

---

## 🎯 Passo 9: Fazer Upload no ESP32

1. Abra Arduino IDE
2. Abra o arquivo `PROTEVE_v5_1_REPLIT.ino`
3. Mude as configurações (passo anterior)
4. Clique em "Upload"

---

## 🎯 Passo 10: Testar!

1. Abra o navegador
2. Vá para seu link do Replit
3. Você deve ver o dashboard!
4. Aguarde o ESP32 enviar dados

---

## ✅ Pronto!

Seu servidor PROTEVE™ está rodando! 🚀

---

## 📞 Problemas?

| Problema | Solução |
|----------|---------|
| Servidor não inicia | Clique "Run" novamente |
| Dashboard não carrega | Verifique se `templates/index.html` existe |
| Erro 404 | Reinicie o servidor |
| ESP32 não conecta | Verifique URL e API Key |

---

**Desenvolvido com ❤️ para PROTEVE™**
