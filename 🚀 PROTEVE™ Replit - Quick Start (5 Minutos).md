# 🚀 PROTEVE™ Replit - Quick Start (5 Minutos)

## ⚡ Resumo Rápido

```
1. Crie conta no Replit (grátis, sem cartão)
2. Crie um novo Repl Python
3. Upload dos arquivos
4. Clique "Run"
5. Configure ESP32
6. Pronto!
```

---

## 📋 Arquivos Necessários

```
✅ proteve_replit_server.py
✅ requirements_replit.txt
✅ .replit
✅ replit.nix
✅ templates/index.html
```

---

## 🎯 Passo a Passo Rápido

### 1️⃣ Criar Conta Replit
- Vá para: https://replit.com
- Clique "Sign up"
- Use Google ou GitHub
- Confirme email

### 2️⃣ Criar Novo Repl
- Clique "+ Create"
- Selecione "Python"
- Nome: `proteve-server`
- Clique "Create Repl"

### 3️⃣ Upload de Arquivos
- Clique no ícone de pasta
- Clique "Upload"
- Selecione os arquivos

### 4️⃣ Criar Pasta Templates
- Clique "New Folder"
- Nome: `templates`
- Crie arquivo: `index.html` com o código HTML

### 5️⃣ Instalar Dependências
- Clique na aba "Shell"
- Digite: `pip install -r requirements_replit.txt`
- Pressione Enter

### 6️⃣ Rodar Servidor
- Clique "Run"
- Aguarde iniciar
- Clique no link que aparece

### 7️⃣ Configurar ESP32
Mude no código:
```cpp
const char* REPLIT_SERVER = "seu-replit-url.repl.co";
const char* REPLIT_API_KEY = "sua-chave-unica";
```

### 8️⃣ Fazer Upload no ESP32
- Use Arduino IDE
- Selecione a porta
- Clique "Upload"

### 9️⃣ Acessar Dashboard
- Abra o navegador
- Vá para: `https://seu-replit-url.repl.co`
- Veja seus dispositivos!

---

## 🔗 Links Importantes

| Item | Link |
|------|------|
| Replit | https://replit.com |
| Seu Repl | `https://proteve-server.seu-usuario.repl.co` |
| Dashboard | `https://proteve-server.seu-usuario.repl.co` |
| API | `https://proteve-server.seu-usuario.repl.co/api/devices` |

---

## 📝 Código HTML para Dashboard

Crie arquivo `templates/index.html` com este conteúdo:

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
        header { background: rgba(255, 255, 255, 0.95); padding: 20px; border-radius: 10px; margin-bottom: 30px; }
        h1 { color: #333; margin-bottom: 10px; }
        .devices-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); gap: 20px; }
        .device-card { background: rgba(255, 255, 255, 0.95); border-radius: 10px; padding: 20px; }
        .device-header { display: flex; justify-content: space-between; margin-bottom: 15px; border-bottom: 2px solid #eee; padding-bottom: 10px; }
        .device-name { font-size: 18px; font-weight: bold; }
        .status-badge { padding: 5px 10px; border-radius: 20px; font-size: 12px; font-weight: bold; }
        .status-online { background: #4caf50; color: white; }
        .status-offline { background: #f44336; color: white; }
        .info-row { display: flex; justify-content: space-between; padding: 8px 0; border-bottom: 1px solid #f0f0f0; font-size: 14px; }
        .device-controls { display: flex; gap: 10px; margin-top: 15px; }
        button { flex: 1; padding: 10px; border: none; border-radius: 5px; cursor: pointer; font-weight: bold; }
        .btn-on { background: #4caf50; color: white; }
        .btn-off { background: #f44336; color: white; }
        .btn-reset { background: #2196F3; color: white; }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🔌 PROTEVE™ Dashboard</h1>
            <p>Gerenciador de Dispositivos Remoto</p>
        </header>
        <div id="devices" class="devices-grid"></div>
    </div>
    <script>
        async function loadDevices() {
            const response = await fetch('/api/devices');
            const devices = await response.json();
            let html = '';
            devices.forEach(d => {
                const status = d.is_online ? 'online' : 'offline';
                const t = d.latest_telemetry || {};
                html += `
                    <div class="device-card">
                        <div class="device-header">
                            <div class="device-name">${d.name}</div>
                            <div class="status-badge status-${status}">${d.is_online ? '🟢 Online' : '🔴 Offline'}</div>
                        </div>
                        <div class="info-row"><span>ID:</span><span>${d.device_id}</span></div>
                        <div class="info-row"><span>Estado:</span><span>${t.estado || 'N/A'}</span></div>
                        <div class="info-row"><span>Relé:</span><span>${t.rele ? '✅ ON' : '❌ OFF'}</span></div>
                        <div class="info-row"><span>Temperatura:</span><span>${t.temperatura ? t.temperatura.toFixed(1) + '°C' : 'N/A'}</span></div>
                        <div class="info-row"><span>Potência:</span><span>${t.potencia_total ? t.potencia_total.toFixed(0) + 'W' : 'N/A'}</span></div>
                        <div class="device-controls">
                            <button class="btn-on" onclick="sendCommand(${d.id}, 'ligar')">🔌 Ligar</button>
                            <button class="btn-off" onclick="sendCommand(${d.id}, 'desligar')">🔴 Desligar</button>
                            <button class="btn-reset" onclick="sendCommand(${d.id}, 'reset')">🔄 Reset</button>
                        </div>
                    </div>
                `;
            });
            document.getElementById('devices').innerHTML = html;
        }
        async function sendCommand(id, cmd) {
            await fetch(`/api/devices/${id}/command`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ comando: cmd })
            });
            loadDevices();
        }
        loadDevices();
        setInterval(loadDevices, 5000);
    </script>
</body>
</html>
```

---

## 🔧 Configuração ESP32

No código `PROTEVE_v5_1_REPLIT.ino`, mude:

```cpp
// Linha ~35
const char* REPLIT_SERVER = "proteve-server.seu-usuario.repl.co";
const char* REPLIT_API_KEY = "proteve-001-chave-unica";
```

---

## ✅ Verificar se Está Funcionando

### No navegador:
```
https://proteve-server.seu-usuario.repl.co
```

### API:
```bash
curl https://proteve-server.seu-usuario.repl.co/api/devices
```

### Health Check:
```bash
curl https://proteve-server.seu-usuario.repl.co/health
```

---

## 🎉 Pronto!

Seu servidor PROTEVE™ está rodando! 🚀

---

## 📞 Problemas?

| Problema | Solução |
|----------|---------|
| Servidor não inicia | Clique "Run" novamente |
| Dashboard não carrega | Verifique se `templates/index.html` existe |
| ESP32 não conecta | Verifique URL e API Key |
| Erro 404 | Reinicie o servidor |

---

**Desenvolvido com ❤️ para PROTEVE™**
