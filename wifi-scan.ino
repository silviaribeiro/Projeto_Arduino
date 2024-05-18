#include <WiFi.h>              // Biblioteca para conectar-se ao Wi-Fi
#include <PubSubClient.h>      // Biblioteca para MQTT
#include <Wire.h>              // Biblioteca para comunicação I2C
#include <LiquidCrystal_I2C.h> // Biblioteca para LCD I2C

// Credenciais do Wi-Fi
const char* ssid = "BOMBO VIVO";
const char* password = "12345BOMBO";

// Endereço do broker MQTT
const char* mqtt_server = "test.mosquitto.org";
int mqtt_port = 1883;

WiFiClient espClient;          // Cliente Wi-Fi
PubSubClient client(espClient);// Cliente MQTT

// Definição dos pinos SDA e SCL do LCD
#define SDA_PIN 21
#define SCL_PIN 22

// Inicializa o LCD com o endereço I2C 0x27, 16 colunas, 2 linhas
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definição dos pinos do sensor ultrassônico
#define TRIG_PIN 13
#define ECHO_PIN 2

// Tópico MQTT para publicar a distância
const char* topic = "sensor/distancia";

void setup() {
  Serial.begin(115200); // Inicializa a comunicação serial
  WiFi.begin(ssid, password); // Conecta ao Wi-Fi

  // Aguarda a conexão com o Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  
  // Exibe informações da conexão Wi-Fi
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());

  // Configura o cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(recebeMsg);
  client.subscribe(topic);

  // Inicializa a comunicação I2C e o LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 1);

  // Configura os pinos do sensor ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

// Função chamada quando uma mensagem é recebida no tópico MQTT inscrito
void recebeMsg(char* topic, byte* mensagem, unsigned int lenght) {
  Serial.print("Mensagem Recebida: ");
  for (int i = 0; i < lenght; i++) {
    Serial.print((char)mensagem[i]);
  }
  Serial.println();
}

void loop() {  
  // Medir a distância com o sensor ultrassônico
  float distance = measureDistance();
  
  // Gera um ID de cliente aleatório para o MQTT
  String clientId = "EPS32Distancia-";
  clientId += String(random(0xffff), HEX);

  // Conecta ao broker MQTT e publica a distância se for menor que 20 cm
  if (client.connect(clientId.c_str())) {          
    if (distance < 20) {
      char message[50]; // Buffer para a mensagem
      snprintf(message, sizeof(message), "%.2f", distance); // Formata a distância como string
      client.publish(topic, message); // Publica a mensagem no tópico
      
      // Exibe a mensagem enviada no LCD
      String Retorno = "Enviado: ";
      Retorno.concat(message);
      lcd.print(Retorno);
      delay(3000);
      lcd.clear();  
    } else {
      // Atualiza o LCD com a distância medida
      updateLCD(distance);
    }
  } else {
    delay(1000);
    lcd.print("bad request");
  }
  
  delay(1000); // Aguarda antes de enviar a próxima mensagem
}

// Atualiza o LCD com a distância medida
void updateLCD(float distance) {
  lcd.clear();
  lcd.print("Distancia: ");
  lcd.print(distance);
  lcd.print(" cm");
  delay(1000); // Aguardar um curto período antes de medir novamente
}

// Função para reconectar ao broker MQTT se a conexão for perdida
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe("test_topic");
    } else {
      delay(5000);
    }
  }
}

// Função para medir a distância usando o sensor ultrassônico
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Mede a duração do pulso no pino Echo
  unsigned long duration = pulseIn(ECHO_PIN, HIGH);

  // Converte a duração em distância (em cm)
  float distance = (duration * 0.034) / 2; // A velocidade do som é de aproximadamente 0.034 cm/µs
  return distance;
}
