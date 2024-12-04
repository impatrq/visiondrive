// Objetos
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin); 
Servo servo;                                                
BlynkTimer timer;

// Parámetros del motor paso a paso
const float maxSpeed = 10000.0;         
const float maxAcceleration = 5000.0;  
const int stepsPerRevolution = 6400;   
const int vueltasFreno = 6;            

// Funciones de control del motor paso a paso
void aplicarFrenado() {
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAcceleration);
  int pasos = vueltasFreno * stepsPerRevolution; 
  stepper.moveTo(pasos);
  stepper.runToPosition();
}

void resetFreno() {
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAcceleration);
  stepper.moveTo(0);
  stepper.runToPosition();
}

// Control del servo
void controlarServo(int angulo) {
  servo.write(angulo);
}