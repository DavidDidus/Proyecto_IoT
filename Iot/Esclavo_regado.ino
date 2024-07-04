#include <Wire.h>

// Umbral de humedad para activar la bomba según la planta
#define HUMEDAD_UMBRAL_ESPINACA 400
#define HUMEDAD_UMBRAL_LECHUGA 350
#define HUMEDAD_UMBRAL_TOMATE 300

// Pines de los sensores de humedad
const int SENSOR_PIN1 = A0;
const int SENSOR_PIN2 = A1;
const int SENSOR_PIN3 = A2;

// Pines de los relés
const int RELAY_PIN1 = 7; // Cambia según tu configuración
const int RELAY_PIN2 = 8; // Cambia según tu configuración
const int RELAY_PIN3 = 9; // Cambia según tu configuración

// Array para almacenar los valores de humedad
byte humedadValues[6];

void setup() {
  Wire.begin(8); // Dirección I2C del esclavo
  Wire.onRequest(requestEvent); // Configura el manejador de eventos para enviar datos
  Serial.begin(9600);

  pinMode(SENSOR_PIN1, INPUT);
  pinMode(SENSOR_PIN2, INPUT);
  pinMode(SENSOR_PIN3, INPUT);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
}

void loop() {
  int humedad1 = analogRead(SENSOR_PIN1);
  int humedad2 = analogRead(SENSOR_PIN2);
  int humedad3 = analogRead(SENSOR_PIN3);

  // Imprime los valores de humedad para depuración
  Serial.print("Humedad1 (Espinaca): ");
  Serial.print(humedad1);
  Serial.print(" Humedad2 (Lechuga): ");
  Serial.print(humedad2);
  Serial.print(" Humedad3 (Tomate): ");
  Serial.println(humedad3);

  // Controla la bomba 1 (Espinaca)
  if (humedad1 < HUMEDAD_UMBRAL_ESPINACA) {
    digitalWrite(RELAY_PIN1, HIGH);
  } else {
    digitalWrite(RELAY_PIN1, LOW);
  }

  // Controla la bomba 2 (Lechuga)
  if (humedad2 < HUMEDAD_UMBRAL_LECHUGA) {
    digitalWrite(RELAY_PIN2, HIGH);
  } else {
    digitalWrite(RELAY_PIN2, LOW);
  }

  // Controla la bomba 3 (Tomate)
  if (humedad3 < HUMEDAD_UMBRAL_TOMATE) {
    digitalWrite(RELAY_PIN3, HIGH);
  } else {
    digitalWrite(RELAY_PIN3, LOW);
  }

  // Almacenar los valores para enviar cuando se solicite
  humedadValues[0] = humedad1 >> 8;
  humedadValues[1] = humedad1 & 0xFF;
  humedadValues[2] = humedad2 >> 8;
  humedadValues[3] = humedad2 & 0xFF;
  humedadValues[4] = humedad3 >> 8;
  humedadValues[5] = humedad3 & 0xFF;

  delay(1000); // Espera un segundo antes de volver a medir
}

void requestEvent() {
  // Enviar los valores de humedad al maestro cuando se solicite
  Wire.write(humedadValues, 6);
}
