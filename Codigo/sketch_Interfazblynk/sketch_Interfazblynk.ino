BLYNK_WRITE(V0) {
  sistemaActivado = param.asInt();  
  if (sistemaActivado == 1) {
    digitalWrite(ledPin, HIGH);  
    controlarServo(80);          
  } else {
    digitalWrite(ledPin, LOW);   
    controlarServo(0);           
    resetFreno();
  }
}

BLYNK_WRITE(V1) {
  velocidadDeseada = param.asInt(); 
}
