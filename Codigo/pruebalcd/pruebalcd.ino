#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección de la pantalla LCD y tamaño
int selected_speed = 0; // Velocidad seleccionada
int current_speed = 0; // Velocidad actual leída del encoder
bool started = false; // Variable para indicar si el programa está iniciado

#define UP_PIN 26 // Pin del botón de subida
#define DOWN_PIN 27 // Pin del botón de bajada
#define START_PIN 13 // Pin del botón de inicio
#define STOP_PIN 12 // Pin del botón de parada

void setup() {
  pinMode(UP_PIN, INPUT_PULLUP); // Configurar el pin del botón de subida con resistencia pull-up interna
  pinMode(DOWN_PIN, INPUT_PULLUP); // Configurar el pin del botón de bajada con resistencia pull-up interna
  pinMode(START_PIN, INPUT_PULLUP); // Configurar el pin del botón de inicio con resistencia pull-up interna
  pinMode(STOP_PIN, INPUT_PULLUP); // Configurar el pin del botón de parada con resistencia pull-up interna

  Wire.begin(); // Inicializar el bus I2C
  lcd.init(); // Inicializar el LCD
  lcd.backlight(); // Encender la retroiluminación
  lcd.clear(); // Limpiar el LCD
  lcd.setCursor(0, 0);
  lcd.print("Select Speed"); // Mostrar "Select Speed" en la parte superior
  lcd.setCursor(0, 1);
  lcd.print("Counter: "); // Mostrar "Counter:" en la parte inferior izquierda
}

void loop() {
  if (!started) {
    if (digitalRead(UP_PIN) == LOW) {
      selected_speed++;
      if (selected_speed > 30) selected_speed = 30;
      updateSpeed();
      delay(200);
    }

    if (digitalRead(DOWN_PIN) == LOW) {
      selected_speed--;
      if (selected_speed < 0) selected_speed = 0;
      updateSpeed();
      delay(200);
    }
  }

  if (digitalRead(START_PIN) == LOW && !started) {
    started = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current Speed");
    lcd.setCursor(8, 1);
    lcd.print(current_speed);
  }

  if (digitalRead(STOP_PIN) == LOW && started) {
    started = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Select Speed");
    lcd.setCursor(0, 1);
    lcd.print("Counter: ");
  }
}

void updateSpeed() {
  lcd.setCursor(9, 1);
  lcd.print("  ");
  lcd.setCursor(9, 1);
  lcd.print(selected_speed);
}