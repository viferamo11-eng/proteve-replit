# 🚀 PROTEVE™ v6.0 - Deployment no Render (Grátis 24/7)

## 📋 Resumo

**Render** é a melhor alternativa para hospedar o servidor PROTEVE™ v6.0 com IA:
- ✅ **Grátis para sempre** (sem limite de tempo)
- ✅ **Sem cartão de crédito** necessário
- ✅ **24/7 online** (não hiberna)
- ✅ **Performance excelente**
- ✅ **Fácil de usar**

---

## 🎯 Passo a Passo (10 Minutos)

### **PASSO 1: Criar Conta no Render**

1. Vá para: https://render.com
2. Clique em **"Sign Up"**
3. Use **GitHub** para fazer login (mais fácil)
4. Autorize o Render a acessar suas repos

### **PASSO 2: Conectar Repositório GitHub**

1. No Render, clique em **"New +"**
2. Selecione **"Web Service"**
3. Clique em **"Connect a repository"**
4. Procure por: `proteve-replit`
5. Clique em **"Connect"**

### **PASSO 3: Configurar Deploy**

Na página de configuração:

| Campo | Valor |
|-------|-------|
| **Name** | `proteve-v6` |
| **Environment** | `Node` |
| **Build Command** | `npm install` |
| **Start Command** | `npm start` |
| **Region** | `São Paulo` (ou sua região) |
| **Plan** | `Free` |

### **PASSO 4: Deploy**

1. Clique em **"Create Web Service"**
2. Aguarde 2-3 minutos
3. Quando terminar, você verá um link como: `https://proteve-v6.onrender.com`

---

## 🔧 Configurar ESP32

No código `PROTEVE_v6_ESP32.ino`, mude:

```cpp
const char* SERVER_URL = "https://seu-servidor.com";
```

Para:

```cpp
const char* SERVER_URL = "https://proteve-v6.onrender.com";
```

Depois faça upload no ESP32.

---

## 📊 Acessar Dashboard

### **Local (na sua rede)**
```
http://192.168.18.100
Usuário: admin
Senha: proteve123
```

### **Remoto (de qualquer lugar)**
```
https://proteve-v6.onrender.com
Usuário: admin
Senha: proteve123
```

---

## 🤖 Recursos de IA Ativados

✅ **Detecção de Anomalias em Tempo Real**
- Consumo fora do padrão
- Desequilíbrio de fases
- Queda de tensão crítica

✅ **Aprendizado de Padrões**
- Aprende consumo médio
- Identifica picos de consumo
- Detecta mudanças de comportamento

✅ **Relatórios Automáticos a Cada 12h**
- Resumo de consumo
- Anomalias detectadas
- Recomendações de otimização

✅ **Insights Inteligentes**
- Sugestões para economizar energia
- Alertas de problemas
- Análise de eficiência

---

## 📱 Compartilhar com Clientes

Você pode compartilhar o link com seus clientes:

```
https://proteve-v6.onrender.com
Usuário: admin
Senha: proteve123
```

Cada cliente pode ter seu próprio dispositivo com um `DEVICE_ID` diferente.

---

## 🆘 Troubleshooting

### **Servidor não inicia**
- Verifique os logs no Render
- Certifique-se de que `npm install` funcionou
- Verifique `package.json`

### **ESP32 não conecta**
- Verifique a URL do servidor
- Certifique-se de que WiFi está conectado
- Verifique os logs do ESP32

### **Dashboard não carrega**
- Verifique autenticação (admin/proteve123)
- Limpe cache do navegador
- Tente em outro navegador

---

## 📈 Monitorar Performance

No Render, você pode ver:
- CPU usage
- Memory usage
- Network traffic
- Logs em tempo real

---

## 🎉 Pronto!

Seu servidor PROTEVE™ v6.0 está **100% online** e **pronto para produção**! 🚀

**Próximos passos:**
1. Teste com o ESP32 local
2. Compartilhe com clientes
3. Monitore os relatórios de IA
4. Otimize baseado nas recomendações

---

## 📞 Suporte

Se tiver problemas:
1. Verifique os logs no Render
2. Verifique a conexão WiFi do ESP32
3. Teste a API manualmente: `curl https://proteve-v6.onrender.com/health`

**Boa sorte! 💪**
