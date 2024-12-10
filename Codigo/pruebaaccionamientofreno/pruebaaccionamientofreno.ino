#include <AccelStepper.h>

const int stepPin = 19;
const int dirPin = 25;
const int buttonPin = 14;

// Definir el motor usando la biblioteca AccelStepper
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

// Configuración para maximizar el torque
const float maxSpeed = 500000.0;       // Velocidad baja para maximizar torque
const float maxAcceleration = 10000.0; // Aceleración moderada para evitar pérdida de pasos

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);    // Botón con resistencia pull-up

  // Configurar el motor con velocidad y aceleración para mayor torque
  stepper.setMaxSpeed(maxSpeed);       // Configura la velocidad baja para maximizar el torque
  stepper.setAcceleration(maxAcceleration); // Configura una aceleración moderada

  Serial.begin(115200);
}

void loop() {
  // Leer el estado del botón
  if (digitalRead(buttonPin) == LOW) {
    // Cuando se presiona el botón, el motor gira en sentido antihorario
    stepper.moveTo(-3200);  // Número de pasos para la rotación deseada
  } else {
    // Cuando se suelta el botón, el motor regresa a la posición inicial
    stepper.moveTo(0);
  }

  // Ejecutar el movimiento del motor
  stepper.run();
}
