#define BLYNK_TEMPLATE_ID "TMPL2qhiOVqKL"
#define BLYNK_TEMPLATE_NAME "Esp32"
#define BLYNK_AUTH_TOKEN "z_WQQExctXxZYm90cLcXuyi1n2bwjFtm"

#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PID_v1.h>

// Pines físicos
const int stepPin = 19;       // Pin de paso del motor paso a paso (freno)
const int dirPin = 25;        // Pin de dirección del motor paso a paso
const int buttonPin = 14;     // Pin del fin de carrera para resetear freno
const int speedSensorPin = 5; // Pin del sensor infrarrojo de velocidad
const int lidarRXPin = 16;    // Pin RX del sensor LiDAR
const int lidarTXPin = 17;    // Pin TX del sensor LiDAR
const int servoPin = 18;      // Pin del servo
const int ledPin = 2;         // LED integrado (GPIO 2)

// Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Cooperadora Profesores";
char pass[] = "Profes_IMPA_2022";

// Objetos
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin); // Motor paso a paso
Servo servo;                                                // Servo
BlynkTimer timer;

// PID
double velocidadDeseada = 0;    // Velocidad seleccionada (en km/h)
double velocidadActual = 0;    // Velocidad medida (en km/h)
double salidaPID = 90;         // Salida del controlador PID (ángulo del servo)

// *Valores iniciales ajustados para PID*
// Nota: Estos valores son aproximados. Ajustarlos según las pruebas.
double Kp = 1.5; // Proporcional: Respuesta inicial al error (más alto, más rápido responde)
double Ki = 0.5; // Integral: Corrige errores acumulados (aumenta suavidad, pero puede oscilar)
double Kd = 0.2; // Derivativo: Suaviza cambios rápidos (reduce oscilaciones)

// Instancia del controlador PID
PID miPID(&velocidadActual, &salidaPID, &velocidadDeseada, Kp, Ki, Kd, DIRECT);

// Parámetros
const float maxSpeed = 10000.0;          // Velocidad del motor paso a paso en pasos por segundo
const float maxAcceleration = 5000.0;   // Aceleración del motor paso a paso
const int stepsPerRevolution = 6400;    // Micropasos por revolución (DM556)
const int vueltasFreno = 6;             // Avance fijo del freno en vueltas completas
const int distanciasSeguras[] = {0, 30, 54, 63, 72, 81, 121, 187, 204, 221, 238, 
                                  330, 418, 444, 470, 496, 606}; // Distancias mínimas seguras

// Variables de estado
bool sistemaActivado = false;
unsigned long pulsosSensor = 0;
float distanciaActual = 0;
bool finDeCarreraActivado = false;

// Función para medir velocidad
void medirVelocidad() {
  static unsigned long tiempoAnterior = 0;
  unsigned long tiempoActual = millis();
  if (digitalRead(speedSensorPin) == LOW) { // Sensor detecta pulso
    pulsosSensor++;
    if (pulsosSensor % 14 == 0) { // Cada 14 pulsos corresponde a 138 cm
      float distancia = 1.38; // 138 cm en metros
      float tiempo = (tiempoActual - tiempoAnterior) / 1000.0; // Tiempo en segundos
      velocidadActual = (distancia / tiempo) * 3.6; // m/s a km/h
      tiempoAnterior = tiempoActual;
    }
  }
}

// Función para medir distancia con LiDAR
void medirDistanciaLidar() {
  if (Serial2.available() > 0) {
    distanciaActual = Serial2.read(); // Lee la distancia medida por LiDAR
  }
}

// Función para frenar
void aplicarFrenado() {
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAcceleration);
  int pasos = vueltasFreno * stepsPerRevolution; // 6 vueltas completas
  stepper.moveTo(pasos);
  stepper.runToPosition();
}

// Función para resetear freno
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

// Control de crucero con PID
void controlarCrucero() {
  medirVelocidad();
  medirDistanciaLidar();

  if (sistemaActivado && !finDeCarreraActivado) {
    // Calcula la salida del PID
    miPID.Compute();

    // Ajusta el servo según la salida del PID
    controlarServo(salidaPID);

    // Frenado en caso de emergencia por LiDAR
    int distanciaSegura = distanciasSeguras[(int)velocidadDeseada];
    if (distanciaActual < distanciaSegura) {
      controlarServo(80); // Deja de acelerar
      aplicarFrenado();   // Activa el freno
    }
  }
}

// Blynk
BLYNK_WRITE(V0) {
  sistemaActivado = param.asInt();  // Actualiza el estado del sistema
  if (sistemaActivado == 1) {
    digitalWrite(ledPin, HIGH);  // Enciende el LED cuando V0 es 1
    controlarServo(80));          // El servo se mueve a la posición inicial (90 grados)
  } else {
    digitalWrite(ledPin, LOW);   // Apaga el LED cuando V0 es 0
    controlarServo(0);           // El servo se mueve a la posición opuesta (0 grados)
    resetFreno();
  }
}

BLYNK_WRITE(V1) {
  velocidadDeseada = param.asInt(); // Actualiza la velocidad deseada
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, lidarRXPin, lidarTXPin);

  pinMode(speedSensorPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); // Define el pin del LED como salida

  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAcceleration);
  servo.attach(servoPin);

  miPID.SetMode(AUTOMATIC);            // Activa el modo automático del PID
  miPID.SetOutputLimits(0, 180);       // Limita la salida a los ángulos del servo

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, controlarCrucero);
}

void loop() {
  Blynk.run();
  timer.run();

  if (digitalRead(buttonPin) == LOW) {
    finDeCarreraActivado = true;
    sistemaActivado = false; // Apagar el sistema
    Blynk.virtualWrite(V0, 0); // Actualizar en la app Blynk
    digitalWrite(ledPin, LOW); // Asegura que el LED se apague
  }
}