#include <LiquidCrystal.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PID_v1.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Pines
const int motorStepPin = 19;  // Pin de paso del motor paso a paso (freno)
const int motorDirPin = 25;   // Pin de dirección del motor paso a paso
const int lidarRXPin = 16;    // Pin RX del sensor LiDAR
const int lidarTXPin = 17;    // Pin TX del sensor LiDAR
const int speedSensorPin = 5; // Pin del sensor infrarrojo de velocidad
const int buttonUpPin = 2;    // Botón para aumentar velocidad
const int buttonDownPin = 3;  // Botón para reducir velocidad
const int buttonStartPin = 12;// Botón de inicio del control de crucero
const int buttonStopPin = 13; // Botón de parada del control de crucero
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

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C del LCD y tamaño (16 columnas x 2 filas)

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

// Configuración de Blynk
#define BLYNK_TEMPLATE_ID           "TMPL28D2nXBoC"
#define BLYNK_TEMPLATE_NAME         "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "okbDgntN5fu-tlE5AH4eve30CM9iZd9f"
#define BLYNK_PRINT Serial

char ssid[] = "CooperadoraProfesores";
char pass[] = "Profes_IMPA_2022";

BlynkTimer timer;

int motorPin = 13; // Pin del motor
int velocidadMotor = 0; // Variable para la velocidad del motor
bool enMarcha = false; // Estado del carro (true para en marcha, false para detenido)

// Control de velocidad
BLYNK_WRITE(V0) { 
  velocidadMotor = param.asInt();
  Serial.print("Velocidad configurada a: ");
  Serial.println(velocidadMotor);
}

// Control de inicio/parada
BLYNK_WRITE(V1) { 
  enMarcha = param.asInt();
  if (enMarcha) {
    iniciarMotor(velocidadMotor);
    Serial.println("Motor iniciado.");
  } else {
    detenerMotor();
    Serial.println("Motor detenido.");
  }
}

void iniciarMotor(int velocidad) {
  analogWrite(motorPin, velocidad); // Ajusta la velocidad del motor usando PWM
}

void detenerMotor() {
  analogWrite(motorPin, 0); // Detiene el motor
}

BLYNK_CONNECTED() {
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

void myTimerEvent() {
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup() {
  // Inicializar la comunicación serial
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, lidarRXPin, lidarTXPin); // Inicializar el HardwareSerial para el LiDAR

  // Inicializar el LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Configurar pines de motor y botones
  pinMode(motorStepPin, OUTPUT);
  pinMode(motorDirPin, OUTPUT);
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(buttonStopPin, INPUT_PULLUP);
  pinMode(finDeCarreraPin, INPUT_PULLUP);

  // Inicializar los PID
  frenadoPID.SetMode(AUTOMATIC);
  velocidadPID.SetMode(AUTOMATIC);

  // Configuración de Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Mostrar pantalla de selección de velocidad
  lcd.setCursor(0, 0);
  lcd.print("Selecciona Vel.");
  lcd.setCursor(0, 1);
  lcd.print("Vel: ");
  lcd.print(selectedSpeed);

  // Configurar el temporizador de Blynk
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run(); // Ejecuta Blynk

  // Manejo de botones de selección de velocidad
  if (digitalRead(buttonUpPin) == LOW) {
    selectedSpeed++;
    if (selectedSpeed > maxSpeed) selectedSpeed = maxSpeed;
    lcd.setCursor(5, 1);
    lcd.print("   "); // Borrar velocidad anterior
    lcd.setCursor(5, 1);
    lcd.print(selectedSpeed);
    delay(200); // Para evitar rebotes del botón
  }

  if (digitalRead(buttonDownPin) == LOW) {
    selectedSpeed--;
    if (selectedSpeed < 0) selectedSpeed = 0;
    lcd.setCursor(5, 1);
    lcd.print("   "); // Borrar velocidad anterior
    lcd.setCursor(5, 1);
    lcd.print(selectedSpeed);
    delay(200); // Para evitar rebotes del botón
  }

  // Actualizar el control de crucero
  if (digitalRead(buttonStartPin) == LOW) {
    controlDeCrucero = true;
  }

  if (digitalRead(buttonStopPin) == LOW) {
    controlDeCrucero = false;
  }

  if (controlDeCrucero) {
    setPointVel = selectedSpeed;
    velocidadPID.Compute();
    inputVel = velocidadActual;
    setPointDist = distanciasSeguras[selectedSpeed];
    frenadoPID.Compute();
    inputDist = dist;
    lcd.setCursor(0, 0);
    lcd.print("Crucero activado");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Crucero desactivado");
  }

  // Control de motor paso a paso según el PID
  if (outputFreno < 0) {
    // Frena el motor si es necesario
    digitalWrite(motorDirPin, LOW);
    for (int i = 0; i < abs(outputFreno); i++) {
      digitalWrite(motorStepPin, HIGH);
      delayMicroseconds(fastSpeed);
      digitalWrite(motorStepPin, LOW);
      delayMicroseconds(fastSpeed);
    }
  }
}
