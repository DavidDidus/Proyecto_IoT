int pin_sensor = 2; // Se define el pin 2 de arduino para la lectura del sensor de nivel
int pin_alarma = 13; //Se define el pin 13 de arduino para representar una alerta
int valor = 0; // Variable que almacenara la lectura del pin 2 

void setup() {
Serial.begin(9600); //Se inicia el puerto serie 
pinMode(pin_sensor, INPUT); //Se declara el pin como entrada
pinMode(pin_alarma, OUTPUT); //Se declara el pin como salida
}

void loop()
{
valor = digitalRead(pin_sensor); // lectura del pin 2 
Serial.println(valor); // Se imprime la lectura del sensor
delay(100);
//Para activar la alarma se pregunta lo siguiente
if(valor == 0){ // Si el sensor esta en el estado 0 se prende le led del pin 13 
digitalWrite(pin_alarma, HIGH);
}
else{ //De lo contrario estará apagado el led del pin 13 
digitalWrite(pin_alarma, LOW);
}
}