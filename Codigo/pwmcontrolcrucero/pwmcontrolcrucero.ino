// Pines
const int potPin = 32;     // Pin del potenciómetro (entrada analógica)
const int ledPin =5;     // Pin del LED

void setup() {
  pinMode(potPin, INPUT);    // Configurar el pin del potenciómetro como entrada
  pinMode(ledPin, OUTPUT);   // Configurar el pin del LED como salida
}

void loop() {
  int potValue = analogRead(potPin); // Leer el valor del potenciómetro (rango 0-4095)

  // Mapear el valor del potenciómetro a la duración del tiempo de encendido y apagado
  int onTime = map(potValue, 0, 4095, 0, 200);  // Tiempo encendido de 0 ms a 50 ms
  int offTime = map(potValue, 0, 4095, 200, 0); // Tiempo apagado de 50 ms a 0 ms

  // Encender el LED
  digitalWrite(ledPin, HIGH);
  delay(onTime); // Espera por el tiempo encendido

  // Apagar el LED
  digitalWrite(ledPin, LOW);
  delay(offTime); // Espera por el tiempo apagado
}
