// PID
double velocidadDeseada = 0;    
double velocidadActual = 0;    
double salidaPID = 90;         

// *Valores iniciales ajustados para PID*
double Kp = 1.5; 
double Ki = 0.5; 
double Kd = 0.2; 

PID miPID(&velocidadActual, &salidaPID, &velocidadDeseada, Kp, Ki, Kd, DIRECT);
