#define BLYNK_TEMPLATE_ID "TMPL2qhiOVqKL"
#define BLYNK_TEMPLATE_NAME "Prender y apagar"
#define BLYNK_DEVICE_NAME "esp32"
#define BLYNK_AUTH_TOKEN "z_WQQExctXxZYm90cLcXuyi1n2bwjFtm"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PID_v1.h>

// Pines físicos
const int motorStepPin = 19;  // Pin de paso del motor paso a paso (freno)
const int motorDirPin = 25;   // Pin de dirección del motor paso a paso
const int lidarRXPin = 16;    // Pin RX del sensor LiDAR
const int lidarTXPin = 17;    // Pin TX del sensor LiDAR
const int speedSensorPin = 5; // Pin del sensor infrarrojo de velocidad
const int finDeCarreraPin = 14; // Pin del fin de carrera para resetear freno

// Parámetros del motor paso a paso
const int stepsPerRevolution = 6400; // 6400 micropasos por revolución (DM 556)
const float avancePorVuelta = 1.905; // Avance del freno por vuelta de la varilla ACME (cm)
const float maxFreno = 10.0;  // Recorrido máximo del freno (10 cm)

// Control de velocidad del motor paso a paso
const int fastSpeed = 40;     // Velocidad rápida del motor (en microsegundos)
const int mediumSpeed = 80;   // Velocidad media del motor
const int slowSpeed = 160;    // Velocidad lenta del motor

// Parámetros PID para frenado
double setPointDist, inputDist, outputFreno;
double Kp = 2.0, Ki = 5.0, Kd = 1.0; // Ajusta estos valores para tu sistema
PID frenadoPID(&inputDist, &outputFreno, &setPointDist, Kp, Ki, Kd, DIRECT);

// Parámetros PID para control de velocidad
double setPointVel, inputVel, outputVel;
double KpVel = 1.0, KiVel = 0.5, KdVel = 0.1; // Ajusta estos valores para tu sistema
PID velocidadPID(&inputVel, &outputVel, &setPointVel, KpVel, KiVel, KdVel, DIRECT);

// Estado del sistema
bool controlDeCrucero = false; // Indica si el control de crucero está activado
int selectedSpeed = 0; // Velocidad seleccionada por el usuario
int maxSpeed = 25; // Velocidad máxima en km/h (25 km/h)

// Variables de velocidad y distancia
int velocidadActual = 0;
int distanciaSegura = 50; // Distancia mínima segura en cm para comenzar a frenar
int distanciaAnterior = 0; // Para calcular la velocidad de acercamiento

// Variables para LiDAR
int dist;  // Distancia medida por el LiDAR
unsigned long lastLidarTime = 0; // Última vez que se midió la distancia
unsigned long lidarInterval = 100; // Intervalo de tiempo entre lecturas del LiDAR (en ms)

// Variables para sensor infrarrojo (velocidad)
unsigned long lastSpeedTime = 0; // Último tiempo registrado por el sensor
unsigned long currentSpeedTime = 0;
int numPulsos = 0; // Contador de pulsos para medir la velocidad

// Distancias de seguridad según la velocidad seleccionada
int distanciasSeguras[26] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 
                             55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 
                             105, 110, 115, 120, 125}; // En cm, ajustar según sea necesario

// Definición de pines virtuales Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iPhone";  // Nombre de tu red Wi-Fi
char pass[] = "maxpechotes";       // Contraseña de tu red Wi-Fi

BlynkTimer timer;

void setup() {
  // Inicializar la comunicación serial
  Serial.begin(9600);
  Serial.println("Iniciando comunicación serial...");
  
  // Inicialización de la comunicación serial para LiDAR
  Serial2.begin(115200, SERIAL_8N1, lidarRXPin, lidarTXPin); // Inicializar el HardwareSerial para el LiDAR

  // Configurar pines de motor y botones
  pinMode(motorStepPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);
  pinMode(finDeCarreraPin, INPUT_PULLUP);

  // Inicializar los PID
  frenadoPID.SetMode(AUTOMATIC);
  velocidadPID.SetMode(AUTOMATIC);

  // Conectar a Blynk
  Blynk.begin(auth, ssid, pass);

  // Configurar el temporizador de Blynk
  timer.setInterval(100L, controlarBlynk);  // Llamar a la función cada 100 ms
}

void loop() {
  Blynk.run();  // Mantener la conexión de Blynk activa
  timer.run();  // Ejecutar funciones del temporizador
}

// Función que se llama periódicamente para controlar Blynk
void controlarBlynk() {
  // Controlar el crucero automático
  if (controlDeCrucero) {
    // Sensar velocidad actual y aplicar PID para aceleración
    velocidadActual = medirVelocidad();
    inputVel = velocidadActual;
    velocidadPID.Compute();

    // Sensar distancia y aplicar frenado
    inputDist = medirDistanciaLidar();
    setPointDist = calcularDistanciaSegura(selectedSpeed); // Calcular la distancia de frenado segura
    frenadoPID.Compute();
    aplicarFrenado(outputFreno); // Aplicar frenado proporcional
  }
}

// Función para medir la velocidad actual (sensor infrarrojo)
int medirVelocidad() {
  // (Implementación de sensor infrarrojo)
  return velocidadActual;
}

// Función para medir la distancia con el sensor LiDAR
int medirDistanciaLidar() {
  // (Implementación del sensor LiDAR)
  return dist;
}

// Función para aplicar el frenado
void aplicarFrenado(double fuerzaFrenado) {
  if (fuerzaFrenado > 0) {
    digitalWrite(motorDirPin, HIGH); // Dirección de frenado
    for (int i = 0; i < stepsPerRevolution * (fuerzaFrenado / avancePorVuelta); i++) {
      digitalWrite(motorStepPin, HIGH);
      delayMicroseconds(slowSpeed);
      digitalWrite(motorStepPin, LOW);
      delayMicroseconds(slowSpeed);
    }
  }
}

// Función para resetear el freno
void resetFreno() {
  digitalWrite(motorDirPin, LOW); // Dirección para soltar el freno
  for (int i = 0; i < stepsPerRevolution * (maxFreno / avancePorVuelta); i++) {
    digitalWrite(motorStepPin, HIGH);
    delayMicroseconds(fastSpeed);
    digitalWrite(motorStepPin, LOW);
    delayMicroseconds(fastSpeed);
  }
}

// Función para calcular la distancia de frenado segura según la velocidad
int calcularDistanciaSegura(int velocidad) {
  if (velocidad > maxSpeed) velocidad = maxSpeed;
  return distanciasSeguras[velocidad];
}

BLYNK_WRITE(V1) {
  selectedSpeed = param.asInt(); // Obtener la velocidad seleccionada desde Blynk
}

BLYNK_WRITE(V2) {
  controlDeCrucero = param.asInt(); // Activar/desactivar control de crucero
}
