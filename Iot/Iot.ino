#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeLib.h>
/*
const char* ssid = "Iphone de David";
const char* password = "coninlomaslindo";
*/
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


// Configuración del pin del botón
const int buttonPin = D1;

// Variables para el estado del botón
int lastButtonState = LOW;
int buttonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Tiempo de anti-rebote
// Variables para el tiempo inicial
time_t tiempoInicial = 0;
unsigned long millisPrevio = 0;



void setup() {
  Serial.begin(115200);

  // Conectar a la red WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  setTime(12, 34, 56, 23, 5, 2024); // hora, minuto, segundo, día, mes, año
 
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
String getHora() {
  // Desplazamiento para Chile
  int timezoneOffset = -4; // UTC-4 para la hora estándar
  time_t localTime = now() + timezoneOffset * SECS_PER_HOUR;
  char buffer[20];
  sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%04d", hour(localTime), minute(localTime), second(localTime), day(localTime), month(localTime), year(localTime));
  return String(buffer);
}


void loop() {
   if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {  // El botón está presionado
        Serial.println("Botón presionado");
        String mensaje = String("Botón presionado ");// + getHora();
        client.publish(mqtt_topic, mensaje.c_str());
       
      }
    }
  }

  lastButtonState = reading;
}