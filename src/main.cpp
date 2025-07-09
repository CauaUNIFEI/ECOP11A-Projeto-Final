 #include <WiFi.h>
 #include <WiFiClientSecure.h>
 #include
 <UniversalTelegramBot.h>
 #include <Servo.h>
 // --- Definições de pinos--
#define TRIG_PIN 4      // Sensor ultrassônico - Trigger
 #define ECHO_PIN 5      // Sensor
 ultrassônico - Echo
 #define SERVO_PIN 14    // Servo motor (trava)
 #define LED_RED 15      //
 LED de alarme
 #define LED_GREEN 12    // LED de destravamento
 #define BUTTON_PIN 13   // Botão
 para abrir manualmente
 #define LEVER_PIN 27    // Alavanca para fechar manualmente
 // --
Wi-Fi e Telegram --
const char* ssid = "SEU_SSID";
 const char* password =
 "SUA_SENHA";
 const char* botToken =
 "7866345535:AAE8SviT6jYuOnr4erhnFDuvTwkGOmK1Pus";
 const char* chatID =
 "1274829496";
 WiFiClientSecure client;
 UniversalTelegramBot bot(botToken,
 client);
 Servo travaServo;
 // --- Variáveis de estado --
bool sistemaAtivo = false;
 bool
 travaAberta = false;
 long tempoUltimaMensagem = 0;
 const long intervaloMensagem = 2000; // 2
 segundos
 // --- Setup --
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
 pinMode(ECHO_PIN, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
 pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEVER_PIN, INPUT_PULLUP);
 travaServo.attach(SERVO_PIN);
  travaServo.write(0); // Trava fechada
  digitalWrite(LED_RED,
 LOW);
  digitalWrite(LED_GREEN, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() !=
 WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setInsecure(); //
 Ignora certificado
  bot.sendMessage(chatID, "Sistema iniciado com sucesso.",
 "");
 }
 // --- Loop Principal --
void loop() {
  // Checa novas mensagens do
Telegram periodicamente
  if (millis() - tempoUltimaMensagem > intervaloMensagem) {
    int
 numNovasMensagens = bot.getUpdates(bot.last_message_received + 1);
    while
 (numNovasMensagens) {
      trataMensagensTelegram();
      numNovasMensagens =
 bot.getUpdates(bot.last_message_received + 1);
    }
    tempoUltimaMensagem = millis();
  }
 // Lógica simples do sistema
  if (sistemaAtivo) {
    float distancia = medirDistancia();
 if (distancia > 0 && distancia < 20.0) {
      digitalWrite(LED_RED, HIGH);
 travaServo.write(0); // Trava fechada
      bot.sendMessage(chatID, "Movimento detectado!
 Alarme acionado.", "");
      delay(3000); // Espera antes de desligar o
 LED/alarm
      digitalWrite(LED_RED, LOW);
    }
  }
  // Abrir trava manualmente pelo botão
 if (digitalRead(BUTTON_PIN) == LOW) {
    abrirTrava();
  }
  // Fechar trava manualmente pela
 alavanca
  if (digitalRead(LEVER_PIN) == LOW) {
    fecharTrava();
  }
 }
 // --- Funções
 Auxiliares --
float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
 digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long
 duracao = pulseIn(ECHO_PIN, HIGH);
  float distancia = duracao * 0.034 / 2;
  return
 distancia;
 }
 void abrirTrava() {
  travaServo.write(90); // Abre
  travaAberta = true;
 digitalWrite(LED_GREEN, HIGH);
  bot.sendMessage(chatID, "Trava aberta.",
 "");
  delay(2000);
  fecharTrava();
 }
 void fecharTrava() {
  travaServo.write(0);
 // Fecha
  travaAberta = false;
  digitalWrite(LED_GREEN, LOW);
  bot.sendMessage(chatID,
 "Trava fechada.", "");
 }
void trataMensagensTelegram() {
  for (int i = 0;
 i < bot.messages.size(); i++) {
    String texto = bot.messages[i].text;
    if (texto ==
 "/ativar") {
      sistemaAtivo = true;
      bot.sendMessage(chatID, "Sistema
 ativado.", "");
    } else if (texto == "/desativar") {
 sistemaAtivo = false;
      bot.sendMessage(chatID, "Sistema desativado.",
 "");
    } else if (texto == "/status") {
      String status =
 sistemaAtivo ? "Ativo" : "Desativado";
      String trava = travaAberta ?
 "Aberta" : "Fechada";
      String msg = "Status do Sistema:\nSistema:
 " + status + "\nTrava: " + trava;
      bot.sendMessage(chatID, msg,
 "");
    } else if (texto == "/abrir") {
      abrirTrava();
    } else if
 (texto == "/fechar") {
      fecharTrava();
    } else if (texto ==
 "/ajuda") {
      String help = "Comandos:\n/ativar - Ativa o
 sistema\n/desativar - Desativa\n/abrir - Abrir trava\n/fechar - Fechar trava\n/status 
Status\n/ajuda - Ajuda";
      bot.sendMessage(chatID, help, "");
    }
  }
 }
