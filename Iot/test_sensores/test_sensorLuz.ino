// Constantes que no cambiarán
const int PIN_SENSOR_LUZ = A0; // Pin de Arduino conectado al pin del sensor de luz
const int PIN_LED_ESPINACA = 3; // Pin de Arduino conectado al pin del LED para espinaca
const int PIN_LED_LECHUGA = 4;  // Pin de Arduino conectado al pin del LED para lechuga
const int PIN_LED_TOMATE = 5;   // Pin de Arduino conectado al pin del LED para tomate

// Umbrales específicos para cada planta (valores en lux)
const int UMBRAL_ESPINACA = 3000;
const int UMBRAL_LECHUGA = 2000;
const int UMBRAL_TOMATE = 5000;

// Variables que cambiarán:
int valorAnalogico;

void setup() {
  // Inicializa la comunicación serial a 9600 bits por segundo:
  Serial.begin(9600);
  pinMode(PIN_LED_ESPINACA, OUTPUT); // Configura el pin de Arduino como salida para espinaca
  pinMode(PIN_LED_LECHUGA, OUTPUT);  // Configura el pin de Arduino como salida para lechuga
  pinMode(PIN_LED_TOMATE, OUTPUT);   // Configura el pin de Arduino como salida para tomate
}

void loop() {
  // Lee la entrada en el pin analógico A0 (valor entre 0 y 1023)
  valorAnalogico = analogRead(PIN_SENSOR_LUZ);

  // Convertir la lectura analógica a lux (ajustar según el sensor específico que estés usando)
  int valorLux = map(valorAnalogico, 0, 1023, 0, 10000);

  Serial.print("Lectura analógica: ");
  Serial.print(valorAnalogico); // La lectura analógica en bruto
  Serial.print(" - Lux: ");
  Serial.println(valorLux); // La lectura convertida a lux

  // Determina la condición de luz basada en umbrales cualitativos
  if (valorAnalogico < 10) {
    Serial.println(" - Oscuro");
  } else if (valorAnalogico < 200) {
    Serial.println(" - Tenue");
  } else if (valorAnalogico < 500) {
    Serial.println(" - Luz");
  } else if (valorAnalogico < 800) {
    Serial.println(" - Brillante");
  } else {
    Serial.println(" - Muy brillante");
  }

  // Controla los LEDs basado en los umbrales específicos para cada planta
  if (valorLux < UMBRAL_ESPINACA) {
    digitalWrite(PIN_LED_ESPINACA, HIGH); // Enciende el LED de espinaca
  } else {
    digitalWrite(PIN_LED_ESPINACA, LOW);  // Apaga el LED de espinaca
  }

  if (valorLux < UMBRAL_LECHUGA) {
    digitalWrite(PIN_LED_LECHUGA, HIGH); // Enciende el LED de lechuga
  } else {
    digitalWrite(PIN_LED_LECHUGA, LOW);  // Apaga el LED de lechuga
  }

  if (valorLux < UMBRAL_TOMATE) {
    digitalWrite(PIN_LED_TOMATE, HIGH); // Enciende el LED de tomate
  } else {
    digitalWrite(PIN_LED_TOMATE, LOW);  // Apaga el LED de tomate
  }

  delay(500); // Espera 500 milisegundos antes de la próxima lectura
}

