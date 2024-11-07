#include <WiFiManager.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <DHT.h>

// Configura os pinos
const int WiFiPin = 15;      // Pino do LED de Wi-Fi
const int MqttPin = 4;       // Pino que será configurado como HIGH quando conectar no MQTT
const int SensorPin = 12;    // Pino de leitura do sensor DHT11 

// Configuração do sensor DHT11
#define DHTTYPE DHT11         // Tipo do sensor DHT11

DHT dht(SensorPin, DHTTYPE); // Inicializa o sensor DHT11 no pino configurado como SensorPin

// Endereço do servidor MQTT e credenciais
const char* mqtt_server = "127.0.0.1";
const int mqtt_port = 18219;
const char* mqtt_user = "teste";
const char* mqtt_pass = "teste";

// Define o fuso horário desejado usando o UTC per second
const long utcOffsetInSeconds = -10800; // GMT-3 = -10800 segundos

// Inicialize o cliente UDP e o NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient espClient;
PubSubClient client(espClient);

// Função de callback quando uma mensagem é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload para uma string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem: ");
  Serial.println(message);

  // Verifica se a mensagem é "Reset"
  if (message == "Reset") {
      WiFiManager wm;
      wm.resetSettings();
      ESP.restart();
  }

  Serial.println();
}

void reconnect() {
  // Loop até conseguir reconectar
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Tenta se conectar
    if (client.connect("ClientID", mqtt_user, mqtt_pass)) {
      Serial.println("Conectado ao broker MQTT");
      // Assine o tópico
      client.subscribe("educ/portas/Status");
      client.subscribe("educ/portas/reset");
      // Publique uma mensagem no tópico
      client.publish("educ/portas/Status", "conectt");

      // Configura o MqttPin como HIGH quando a ESP32 conectar ao MQTT
      digitalWrite(MqttPin, HIGH);
    } else {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" tenta reconectar em 5 segundos");
      // Espera 5 segundos antes de tentar se reconectar
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando sistema.");

  // Configura os pinos
  pinMode(WiFiPin, OUTPUT);  // Configura o pino 15 como saída para o LED
  pinMode(MqttPin, OUTPUT);  // Configura o pino 4 como saída para indicar o status da conexão MQTT
  pinMode(SensorPin, INPUT); // Configura o pino de leitura do DHT11 como INPUT

  digitalWrite(WiFiPin, HIGH);
  delay(1000);
  digitalWrite(WiFiPin, LOW);

  // Instancia o WiFiManager
  WiFiManager wm;

  // Tenta conectar usando as credenciais salvas ou inicia um AP para configuração
  bool res = wm.autoConnect("IOTMCL", "##2990##"); // AP protegido por senha
  
  if (!res) {
    Serial.println("Falha ao conectar. Reiniciando...");
    digitalWrite(WiFiPin, LOW);  // Garante que o LED esteja apagado em caso de falha
    ESP.restart();          // Reinicie se a conexão falhar
  } else {
    Serial.println("Conectado com sucesso!");
    // Acende o LED após a conexão bem-sucedida
    digitalWrite(WiFiPin, HIGH);
  }
  
  // Inicialize o cliente NTP
  timeClient.begin();
  
  // Sincronize o tempo com o servidor NTP
  timeClient.update();
  
  // Inicializa o sensor DHT11
  dht.begin();
  
  // Configura o servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Conecta-se ao broker MQTT
  reconnect();
}

void loop() {
  // Atualiza o tempo
  timeClient.update();

  // Lê a umidade do sensor DHT11
  float humidity = dht.readHumidity();  // Lê a umidade em porcentagem
  
  // Verifica se a leitura foi bem-sucedida
  if (isnan(humidity)) {
    Serial.println("Falha ao ler o sensor DHT11!");
    return; // Se a leitura falhou, sai da função loop
  }

  // Envia a porcentagem de umidade para o MQTT
  String humidityStr = String(humidity, 0);  // Converte para string (sem casas decimais)
  Serial.print("Umidade: ");
  Serial.print(humidityStr);
  Serial.println("%");
  
  // Publica a umidade no tópico MQTT
  client.publish("mig/portas/Status", humidityStr.c_str());

  // Verifica se está conectado e mantém a conexão
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Aguarda um tempo antes de fazer nova leitura
  delay(10000);  // Aguarda 10 segundos antes de ler o sensor novamente
}
