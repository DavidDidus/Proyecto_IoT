#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <TimeLib.h>

// Configuración de red WiFi
const char* ssid = "S22";
const char* password = "12341234";

// Configuración del broker MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "test_invernadero/topic";
const char* mqtt_instrucciones_topic = "test_invernadero/instrucciones";

// Cliente WiFi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Configuración del pin del botón
const int buttonPin = D1;
const int rainPin = A0;


// Variables para el estado del botón
int lastButtonState = LOW;
int buttonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Tiempo de anti-rebote

// Variables para el sensor de lluvia
int thresholdValue = 100;

void setup() {
  Serial.begin(115200);

  pinMode(rainPin, INPUT);
  pinMode(buttonPin, INPUT);

  // Conectar a la red WiFi
  setup_wifi();

  // Configurar el cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallBack(callback);
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

  if (message == "REGAR") {
    Serial.println("Instrucción recibida: Regar la planta");
    // Aquí puedes agregar el código para activar la bomba de riego
  } else if (message == "NO_REGAR") {
    Serial.println("Instrucción recibida: No regar la planta");
    // Aquí puedes agregar el código para desactivar la bomba de riego
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lectura y procesamiento del estado del botón
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {  // El botón está presionado
        Serial.println("Botón presionado");
        String mensaje = String("Botón presionado ");
        client.publish(mqtt_topic, mensaje.c_str());
      }
    }
  }
  lastButtonState = reading;

  // Lectura y procesamiento del sensor de lluvia
  int sensorValue = analogRead(rainPin);
  Serial.print("Valor del sensor: ");
  Serial.print(sensorValue);
  String sensorMsg = "";

  if (sensorValue < thresholdValue) {
    Serial.println(" - No necesita riego");
    sensorMsg = String(sensorValue);
  } else {
    Serial.println(" - Tiempo de regar la planta");
    sensorMsg = String(sensorValue);
  }

  // Publicar el estado del sensor a través de MQTT
  client.publish(mqtt_topic, sensorMsg.c_str());

  delay(500);
}
