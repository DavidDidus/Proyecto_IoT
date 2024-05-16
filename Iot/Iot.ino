#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuración de red WiFi
const char* ssid = "S22";
const char* password = "12341234";

// Configuración del broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "test_invernadero/topic";

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Conectar a la red WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
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
    } else {
      Serial.print("fallido, rc=");
      Serial.print(client.state());
      Serial.println(" intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Publicar un mensaje cada 5 segundos
  static unsigned long lastMsg = 0;
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    String msg = "Hello MQTT from Arduino";
    Serial.print("Publicando mensaje: ");
    Serial.println(msg);
    client.publish(mqtt_topic, msg.c_str());
  }
}