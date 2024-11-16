#define BLYNK_TEMPLATE_ID    "TMPL28QAGa40H"
#define BLYNK_TEMPLATE_NAME  "Quickstart Template"
#define BLYNK_AUTH_TOKEN     "Pzvgtd0qTLjnfOla8uMFlXH4K0lAfHLF"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Cooperadora Profesores";         // Nombre de tu red Wi-Fi
char pass[] = "Profes_IMPA_2022";               // Contraseña de tu red Wi-Fi

int ledPin = 2;  // El LED está conectado al pin GPIO 2 de la ESP32

BlynkTimer timer;  // Crear un temporizador de Blynk

void setup() {
  // Iniciar la comunicación serial
  Serial.begin(115200);

  // Configurar el pin del LED
  pinMode(ledPin, OUTPUT);

  // Conectar a Wi-Fi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configurar el pin de Blynk (en la app, conecta el widget de botón al pin virtual V1)
  Blynk.virtualWrite(V1, LOW);  // Inicia el LED apagado
}

void loop() {
  Blynk.run();  // Mantener la conexión con Blynk
}

// Este es el método que se llama cuando se presiona el botón en la app de Blynk
BLYNK_WRITE(V1) {
  int pinValue = param.asInt();  // Obtener el valor del botón
  if (pinValue == 1) {
    digitalWrite(ledPin, HIGH);  // Prender el LED
  } else {
    digitalWrite(ledPin, LOW);   // Apagar el LED
  }
}
