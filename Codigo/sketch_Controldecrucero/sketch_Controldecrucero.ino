void controlarCrucero() {
  medirVelocidad();
  medirDistanciaLidar();

  if (sistemaActivado && !finDeCarreraActivado) {
    miPID.Compute();
    controlarServo(salidaPID);

    int distanciaSegura = distanciasSeguras[(int)velocidadDeseada];
    if (distanciaActual < distanciaSegura) {
      controlarServo(80); 
      aplicarFrenado();   
    }
  }
}
