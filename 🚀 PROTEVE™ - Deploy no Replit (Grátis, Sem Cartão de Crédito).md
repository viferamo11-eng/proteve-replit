# 🚀 PROTEVE™ - Deploy no Replit (Grátis, Sem Cartão de Crédito)

## 📋 O Que Você Vai Conseguir

✅ **Servidor rodando 24/7 GRÁTIS**
✅ **Gerenciar múltiplos ESP32s remotamente**
✅ **Dashboard web para clientes**
✅ **API para integração**
✅ **Banco de dados para histórico**
✅ **Sem limite de tempo**
✅ **SEM CARTÃO DE CRÉDITO**

---

## 🎯 Arquitetura Final

```
┌─────────────────────────────────────────────────────────┐
│           REPLIT (Grátis, Sem Cartão)                   │
│                                                         │
│  ├─ Servidor Python (Flask)                            │
│  ├─ Banco de Dados (SQLite)                            │
│  ├─ Dashboard Web                                       │
│  └─ API REST                                            │
└────┬────────────────────────────────────────────────────┘
     │
     ├─────────────────────────────────────────────┐
     │                                             │
     ▼                                             ▼
┌──────────────────┐                    ┌──────────────────┐
│  ESP32 Cliente 1 │                    │  ESP32 Cliente 2 │
│  (Envia dados)   │                    │  (Envia dados)   │
│  (Recebe cmds)   │                    │  (Recebe cmds)   │
└──────────────────┘                    └──────────────────┘
```

---

## 📋 Pré-Requisitos

- ✅ Conta Google ou GitHub (para login no Replit)
- ✅ Arquivo: `proteve_replit_server.py`
- ✅ Arquivo: `PROTEVE_v5_1_ORACLE_CLOUD.ino` (mesmo do Oracle, funciona igual)
- ✅ Arquivo: `.replit`
- ✅ Arquivo: `requirements_replit.txt`

---

## 🔧 PASSO 1: Criar Conta no Replit

### 1.1 Acesse o site
- Vá para: https://replit.com

### 1.2 Clique em "Sign up"
- Use Google ou GitHub
- **Importante**: Totalmente GRÁTIS, sem cartão de crédito!

### 1.3 Confirme seu email
- Você receberá um email de confirmação
- Clique no link

### 1.4 Faça login
- Pronto! Você está dentro do Replit

---

## 📁 PASSO 2: Criar um Novo Repl

### 2.1 Na página inicial do Replit:
1. Clique em **"+ Create"**
2. Selecione **"Python"**
3. Dê um nome: `proteve-server`
4. Clique em **"Create Repl"**

### 2.2 Aguarde carregar
- Você verá um editor de código
- Pronto para começar!

---

## 📥 PASSO 3: Upload dos Arquivos

### 3.1 Estrutura de pastas esperada:

```
proteve-server/
├── proteve_replit_server.py
├── requirements_replit.txt
├── .replit
├── replit.nix
└── templates/
    └── index.html
```

### 3.2 Upload via Replit:

1. **Clique no ícone de pasta** (lado esquerdo)
2. **Clique em "Upload"**
3. **Selecione os arquivos**:
   - `proteve_replit_server.py`
   - `requirements_replit.txt`
   - `.replit`
   - `replit.nix`

### 3.3 Criar pasta templates:

1. **Clique em "New Folder"**
2. **Nome**: `templates`
3. **Crie arquivo**: `index.html` dentro da pasta

---

## 🎨 PASSO 4: Criar Dashboard HTML

### 4.1 Abra o arquivo `templates/index.html`

### 4.2 Cole este conteúdo:

```html
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PROTEVE™ Dashboard</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        header {
            background: rgba(255, 255, 255, 0.95);
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 30px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }
        
        h1 {
            color: #333;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #666;
            font-size: 14px;
        }
        
        .devices-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .device-card {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            transition: transform 0.3s ease;
        }
        
        .device-card:hover {
            transform: translateY(-5px);
        }
        
        .device-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 15px;
            border-bottom: 2px solid #eee;
            padding-bottom: 10px;
        }
        
        .device-name {
            font-size: 18px;
            font-weight: bold;
            color: #333;
        }
        
        .status-badge {
            padding: 5px 10px;
            border-radius: 20px;
            font-size: 12px;
            font-weight: bold;
        }
        
        .status-online {
            background: #4caf50;
            color: white;
        }
        
        .status-offline {
            background: #f44336;
            color: white;
        }
        
        .device-info {
            margin-bottom: 15px;
        }
        
        .info-row {
            display: flex;
            justify-content: space-between;
            padding: 8px 0;
            border-bottom: 1px solid #f0f0f0;
            font-size: 14px;
        }
        
        .info-label {
            color: #666;
            font-weight: 500;
        }
        
        .info-value {
            color: #333;
            font-weight: bold;
        }
        
        .device-controls {
            display: flex;
            gap: 10px;
            margin-top: 15px;
        }
        
        button {
            flex: 1;
            padding: 10px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-weight: bold;
            transition: background 0.3s ease;
        }
        
        .btn-on {
            background: #4caf50;
            color: white;
        }
        
        .btn-on:hover {
            background: #45a049;
        }
        
        .btn-off {
            background: #f44336;
            color: white;
        }
        
        .btn-off:hover {
            background: #da190b;
        }
        
        .btn-reset {
            background: #2196F3;
            color: white;
        }
        
        .btn-reset:hover {
            background: #0b7dda;
        }
        
        .loading {
            text-align: center;
            color: white;
            font-size: 18px;
            padding: 40px;
        }
        
        .error {
            background: #f44336;
            color: white;
            padding: 15px;
            border-radius: 5px;
            margin-bottom: 20px;
        }
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
        // Carregar dispositivos
        async function loadDevices() {
            try {
                const response = await fetch('/api/devices');
                const devices = await response.json();
                
                document.getElementById('loading').style.display = 'none';
                
                if (devices.length === 0) {
                    document.getElementById('devices').innerHTML = '<p>Nenhum dispositivo registrado</p>';
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
        
        // Enviar comando
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
        
        // Carregar dispositivos ao iniciar
        loadDevices();
        
        // Atualizar a cada 5 segundos
        setInterval(loadDevices, 5000);
    </script>
</body>
</html>
```

---

## ⚙️ PASSO 5: Instalar Dependências

### 5.1 No Replit:

1. **Clique na aba "Shell"** (lado direito)
2. **Digite**:
   ```bash
   pip install -r requirements_replit.txt
   ```
3. **Pressione Enter**
4. **Aguarde terminar**

---

## ▶️ PASSO 6: Rodar o Servidor

### 6.1 No Replit:

1. **Clique no botão "Run"** (topo)
2. **Você verá**:
   ```
   * Running on http://0.0.0.0:3000
   ```
3. **Pronto! Seu servidor está rodando!**

### 6.2 Acessar o Dashboard:

- Clique no link que aparece (algo como `https://proteve-server.seu-usuario.repl.co`)
- Você verá o dashboard!

---

## 🔧 PASSO 7: Configurar ESP32

### 7.1 Abra o Arduino IDE

### 7.2 Abra o arquivo:
- `PROTEVE_v5_1_ORACLE_CLOUD.ino`

### 7.3 Mude estas linhas:

```cpp
// Linha ~35
const char* ORACLE_SERVER = "proteve-server.seu-usuario.repl.co";  // Seu URL Replit
const char* ORACLE_API_KEY = "proteve-001-chave-unica";  // Gere uma chave
```

### 7.4 Faça upload para o ESP32

---

## 📱 PASSO 8: Testar Tudo

### 8.1 Verificar se o servidor está rodando:

- Abra o navegador
- Vá para: `https://proteve-server.seu-usuario.repl.co`
- Você deve ver o dashboard

### 8.2 Verificar API:

```bash
curl https://proteve-server.seu-usuario.repl.co/api/devices
```

---

## 🎉 PASSO 9: Compartilhar com Clientes

Dê para seus clientes:

```
URL: https://proteve-server.seu-usuario.repl.co
```

Eles podem acessar de qualquer lugar do mundo!

---

## 🔄 Manter Servidor Rodando 24/7

### Opção 1: Replit Free (Recomendado)
- ✅ Roda enquanto você está usando
- ✅ Hiberna quando inativo por 1 hora
- ✅ Grátis!

### Opção 2: Replit Pro (Opcional)
- ✅ Roda 24/7
- ✅ Mais poder computacional
- ✅ Pago (mas barato)

Para usar Replit Pro:
1. Clique em seu avatar (canto superior direito)
2. Clique em "Upgrade to Pro"
3. Escolha o plano

---

## 🛠️ Comandos Úteis

### Ver logs:
```bash
# Na aba Shell do Replit
tail -f replit.log
```

### Reiniciar servidor:
```bash
# Clique em "Stop" e depois "Run"
```

### Ver status:
```bash
curl https://proteve-server.seu-usuario.repl.co/health
```

---

## 🚨 Troubleshooting

### Servidor não inicia:
1. Verifique se todos os arquivos estão no lugar
2. Verifique se as dependências foram instaladas
3. Clique em "Run" novamente

### Dashboard não carrega:
1. Verifique se o servidor está rodando
2. Tente atualizar a página (F5)
3. Verifique o console (F12) para erros

### ESP32 não conecta:
- Verifique o URL do servidor (copie exatamente)
- Verifique a API Key
- Verifique o WiFi do ESP32

### Erro 404:
- Verifique se o arquivo `index.html` está em `templates/`
- Verifique se o servidor está rodando

---

## 📊 Limitações do Replit Free

| Item | Limite |
|------|--------|
| RAM | 0.5 GB |
| CPU | 1 vCPU |
| Storage | Ilimitado |
| Tempo de execução | Hibernação após 1h inativo |
| Banda | Ilimitada |

**Suficiente para 5-10 ESP32s!**

---

## 💰 Custo

| Plano | Preço | Ideal Para |
|-------|-------|-----------|
| **Free** | **R$ 0** | Testes, 5-10 ESP32s |
| **Pro** | ~R$ 30/mês | Produção, 24/7 |

---

## 🎉 Pronto!

Seu servidor PROTEVE™ está rodando no Replit! 🚀

### Próximos passos:
1. Configure o ESP32 com o URL do Replit
2. Teste com um ESP32
3. Compartilhe a URL com clientes
4. Monitore via dashboard

---

## 📞 Suporte

Se tiver problemas:

1. Verifique os logs no Replit
2. Teste a API: `curl https://seu-replit-url/health`
3. Reinicie o servidor: clique "Stop" e "Run"

---

**Desenvolvido com ❤️ para PROTEVE™**
