#define BLYNK_TEMPLATE_ID "TMPL2qhiOVqKL"
#define BLYNK_TEMPLATE_NAME "Esp32"
#define BLYNK_AUTH_TOKEN "z_WQQExctXxZYm90cLcXuyi1n2bwjFtm"

#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <PID_v1.h>

// Pines físicos
const int stepPin = 19;       
const int dirPin = 25;        
const int buttonPin = 14;     
const int speedSensorPin = 5; 
const int lidarRXPin = 16;    
const int lidarTXPin = 17;    
const int servoPin = 18;      
const int ledPin = 2;         

// Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Cooperadora Profesores";
char pass[] = "Profes_IMPA_2022";