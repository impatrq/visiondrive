void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, lidarRXPin, lidarTXPin);

  pinMode(speedSensorPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT); 

  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(maxAcceleration);
  servo.attach(servoPin);

  miPID.SetMode(AUTOMATIC);            
  miPID.SetOutputLimits(0, 180);       

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(100L, controlarCrucero);
}

void loop() {
  Blynk.run();
  timer.run();

  if (digitalRead(buttonPin) == LOW) {
    finDeCarreraActivado = true;
    sistemaActivado = false; 
    Blynk.virtualWrite(V0, 0); 
    digitalWrite(ledPin, LOW); 
  }
}
