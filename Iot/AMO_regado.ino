#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <TimeLib.h>

// Configuración de red WiFi
const char* ssid = "FROST";
const char* password = "VIFLO2604";

// Configuración del broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "test_invernadero/topic";
const char* mqtt_instrucciones_topic = "test_invernadero/instrucciones";

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

int humedad1 = 0;
int humedad2 = 0;
int humedad3 = 0;

void setup() {
  Serial.begin(115200);

  // Conectar a la red WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  setTime(12, 34, 56, 23, 5, 2024); // hora, minuto, segundo, día, mes, año

  // Configurar la conexión I2C
  Wire.begin(D2, D1); // Configura el ESP8266 como maestro I2C
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Bucle hasta que el cliente MQTT esté conectado
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (client.connect("ArduinoClient")) {
      Serial.println("conectado");

      // Suscribirse al tema
      client.subscribe(mqtt_topic);
      client.subscribe(mqtt_instrucciones_topic);
    } else {
      Serial.print("fallido, rc=");
      Serial.print(client.state());
      Serial.println(" intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

String getHora() {
  // Desplazamiento para Chile
  int timezoneOffset = -4; // UTC-4 para la hora estándar
  time_t localTime = now() + timezoneOffset * SECS_PER_HOUR;
  char buffer[20];
  sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%04d", hour(localTime), minute(localTime), second(localTime), day(localTime), month(localTime), year(localTime));
  return String(buffer);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer datos de humedad del Arduino Uno (esclavo) a través de I2C
  Wire.requestFrom(8, 6); // Solicita 6 bytes del esclavo con dirección 8
  if (Wire.available() == 6) {
    humedad1 = Wire.read() << 8;
    humedad1 |= Wire.read();
    humedad2 = Wire.read() << 8;
    humedad2 |= Wire.read();
    humedad3 = Wire.read() << 8;
    humedad3 |= Wire.read();
  }

  // Publicar los datos de humedad a través de MQTT
  String mensaje = "Humedad1: " + String(humedad1) + " Humedad2: " + String(humedad2) + " Humedad3: " + String(humedad3);
  client.publish(mqtt_topic, mensaje.c_str());

  // Imprimir los valores de humedad para depuración
  Serial.println(mensaje);

  delay(1000); // Espera un segundo antes de volver a medir
}
