void medirVelocidad() {
  static unsigned long tiempoAnterior = 0;
  unsigned long tiempoActual = millis();
  if (digitalRead(speedSensorPin) == LOW) { 
    pulsosSensor++;
    if (pulsosSensor % 14 == 0) { 
      float distancia = 1.38; 
      float tiempo = (tiempoActual - tiempoAnterior) / 1000.0; 
      velocidadActual = (distancia / tiempo) * 3.6; 
      tiempoAnterior = tiempoActual;
    }
  }
}

void medirDistanciaLidar() {
  if (Serial2.available() > 0) {
    distanciaActual = Serial2.read(); 
  }
}
