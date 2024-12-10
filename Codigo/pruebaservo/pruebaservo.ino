#include <ESP32Servo.h> // Biblioteca para controlar servos en ESP32

// Definir el pin del servo
const int servoPin = 18;

// Crear un objeto Servo
Servo myServo;

// Variables para el movimiento del servo
int angle = 0;    // Ángulo actual del servo
int step = 1;     // Paso para cambiar el ángulo (+1 o -1)

// Configurar el servo y el pin
void setup() {
  myServo.attach(servoPin); // Asociar el servo al pin 18
}

// Controlar el movimiento del servo
void loop() {
  myServo.write(angle); // Enviar el ángulo actual al servo
  delay(15);            // Pequeña pausa para permitir el movimiento del servo

  // Cambiar el ángulo
  angle += step;

  // Cambiar dirección al llegar a los extremos (0° o 180°)
  if (angle >= 90 || angle <= 0) {
    step = -step;
  }
}