# 🔌 PROTEVE™ v6.0 - Sistema Inteligente de Monitoramento Elétrico

![Version](https://img.shields.io/badge/version-6.0.0-green)
![License](https://img.shields.io/badge/license-MIT-blue)
![Status](https://img.shields.io/badge/status-production-brightgreen)

## 📋 O Que É?

**PROTEVE™ v6.0** é um sistema profissional e inteligente para monitoramento remoto de dispositivos elétricos ESP32. Utiliza **IA avançada** para detectar anomalias, aprender padrões de consumo e gerar relatórios automáticos.

### ✨ Principais Características

- 🤖 **IA Integrada** - Detecta anomalias em tempo real
- 📊 **Dashboard Moderno** - Interface profissional com padrão Schneider
- 📈 **Relatórios Automáticos** - A cada 12 horas
- 🔔 **Alertas Inteligentes** - Notificações de consumo anômalo
- 🌐 **Acesso Remoto** - De qualquer lugar do mundo
- 🔐 **Segurança** - Autenticação HTTP Basic
- 📱 **Responsivo** - Funciona em desktop e mobile
- 💾 **Histórico Completo** - Armazena todos os dados

---

## 🚀 Quick Start (2 Minutos)

### 1. **Clonar Repositório**
```bash
git clone https://github.com/seu-usuario/proteve-replit.git
cd proteve-replit
```

### 2. **Instalar Dependências**
```bash
npm install
```

### 3. **Rodar Localmente**
```bash
npm start
```

### 4. **Acessar Dashboard**
```
http://localhost:3000
Usuário: admin
Senha: proteve123
```

---

## 🌐 Deploy no Render (Recomendado)

Veja o arquivo `DEPLOYMENT_RENDER.md` para instruções completas.

**Resumo:**
1. Crie conta no Render (grátis)
2. Conecte seu repositório GitHub
3. Configure e deploy
4. Pronto! Seu servidor está online 24/7

---

## 📡 Integração com ESP32

### **Código do ESP32**

Use o arquivo `PROTEVE_v6_ESP32.ino` incluído neste repositório.

### **Configuração**

No código ESP32, mude:

```cpp
const char* WIFI_SSID = "Seu-WiFi";
const char* WIFI_PASSWORD = "Sua-Senha";
const char* SERVER_URL = "https://seu-servidor.com";
const char* DEVICE_ID = "PROTEVE-001";
```

### **Upload**

1. Abra o Arduino IDE
2. Instale a biblioteca ESP32
3. Abra `PROTEVE_v6_ESP32.ino`
4. Configure a porta e placa
5. Clique em "Upload"

---

## 🤖 Recursos de IA

### **Detecção de Anomalias**

O sistema detecta automaticamente:
- ❌ Consumo acima do normal
- ❌ Consumo abaixo do normal
- ❌ Desequilíbrio entre fases
- ❌ Queda de tensão crítica
- ❌ Surtos de tensão

### **Aprendizado de Padrões**

A IA aprende:
- 📊 Consumo médio do dispositivo
- 📈 Picos de consumo
- 🔄 Padrões de uso
- 🎯 Comportamento normal

### **Insights Inteligentes**

Recomendações automáticas para:
- 💡 Economizar energia
- ⚡ Melhorar eficiência
- 🔧 Otimizar cargas
- 🛡️ Prevenir problemas

---

## 📊 API Endpoints

### **Telemetria**
```bash
POST /api/telemetry
Content-Type: application/json

{
  "id": "PROTEVE-001",
  "A": { "v": 220.5, "i": 12.3, "p": 2640, "fp": 0.92 },
  "B": { "v": 220.1, "i": 12.1, "p": 2640, "fp": 0.92 },
  "C": { "v": 219.8, "i": 12.4, "p": 2640, "fp": 0.92 }
}
```

### **Dados do Dispositivo**
```bash
GET /api/device/:id
Authorization: Basic YWRtaW46cHJvdGV2ZTEyMw==
```

### **Enviar Comando**
```bash
POST /api/command/:id
Authorization: Basic YWRtaW46cHJvdGV2ZTEyMw==

{ "command": "ligar" }
```

### **Gerar Relatório**
```bash
GET /api/report/:id
Authorization: Basic YWRtaW46cHJvdGV2ZTEyMw==
```

### **Health Check**
```bash
GET /health
```

---

## 📁 Estrutura do Projeto

```
proteve-replit/
├── server.js                    # Servidor principal com IA
├── package.json                 # Dependências
├── public/
│   └── index.html              # Dashboard HTML
├── PROTEVE_v6_ESP32.ino        # Código ESP32
├── DEPLOYMENT_RENDER.md        # Guia de deployment
├── README.md                   # Este arquivo
└── .gitignore
```

---

## 🔐 Segurança

- ✅ Autenticação HTTP Basic
- ✅ Credenciais padrão: `admin` / `proteve123`
- ✅ **MUDE AS CREDENCIAIS** em produção!
- ✅ HTTPS em produção
- ✅ Validação de dados de entrada

---

## 📊 Banco de Dados

O sistema usa **JSON em arquivo** (`proteve_db.json`) para armazenar:
- Telemetria (últimos 10.000 registros)
- Anomalias (últimos 1.000 registros)
- Relatórios (histórico completo)
- Padrões de IA

Para produção, considere migrar para **MongoDB** ou **PostgreSQL**.

---

## 🛠️ Desenvolvimento

### **Instalar Dependências**
```bash
npm install
```

### **Modo Desenvolvimento**
```bash
npm run dev
```

### **Build para Produção**
```bash
npm run build
```

---

## 📈 Monitoramento

### **Logs em Tempo Real**
```bash
tail -f proteve_db.json
```

### **Verificar Saúde do Servidor**
```bash
curl https://seu-servidor.com/health
```

---

## 🆘 Troubleshooting

### **ESP32 não conecta ao servidor**
- Verifique a URL do servidor
- Verifique WiFi
- Verifique firewall

### **Dashboard não carrega**
- Limpe cache do navegador
- Verifique autenticação
- Verifique console do navegador

### **Anomalias não detectadas**
- Aguarde 10+ amostras para treinar IA
- Verifique dados de entrada
- Verifique logs do servidor

---

## 📞 Suporte

Para problemas ou sugestões:
1. Verifique os logs
2. Consulte a documentação
3. Abra uma issue no GitHub

---

## 📄 Licença

MIT License - Veja LICENSE.md para detalhes

---

## 🎉 Créditos

Desenvolvido com ❤️ para PROTEVE™

**Versão:** 6.0.0  
**Data:** Janeiro 2026  
**Status:** Production Ready ✅

---

## 🚀 Próximos Passos

1. Deploy no Render
2. Configurar ESP32
3. Testar comunicação
4. Compartilhar com clientes
5. Monitorar relatórios de IA
6. Otimizar baseado em insights

**Boa sorte! 💪**
