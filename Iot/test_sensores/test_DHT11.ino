#include <ESP8266WiFi.h>
#include "DHT.h"

// Configuración del sensor DHT
#define DHTPIN 2 // GPIO2 corresponde al pin D4 en el ESP8266
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Inicia el monitor serial
  Serial.begin(115200);
  Serial.println("Inicializando...");

  // Inicia el sensor DHT
  dht.begin();
  Serial.println("Sensor DHT inicializado.");
}

void loop() {
  // Lee los datos del sensor DHT
  Serial.println("Leyendo datos del DHT...");
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verifica si la lectura falló y muestra un error
  if (isnan(h) || isnan(t)) {
    Serial.println("Error al leer del sensor DHT");
  } else {
    // Muestra los resultados en el monitor serial
    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
  }

  // Espera 10 segundos antes de la próxima lectura
  delay(10000);
}

}
