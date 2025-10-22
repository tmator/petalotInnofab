#include <PID_v1.h>


double T;          //current temp

bool F = false;
bool Fc = false;
bool Fi = false; 

double Output;  //pid output

PID myPID(&T, &Output, &To, Kp, Ki, Kd, DIRECT);

double tempLastSample;
double tempLastFilament;
double tempLastNoFilament;
double tempLastStart;

const float R_FIXED = 100000.0;     // Résistance fixe de 100kΩ
const float BETA = 3950.0;          // Coefficient Beta de la thermistance (à ajuster selon le modèle)
const float T0 = 298.15;            // 25°C en Kelvin
const float R0 = 100000.0;          // 100kΩ à 25°C

//thermistor
float logR2, R2;
//steinhart-hart coeficients for thermistor
float c1 = 0.8438162826e-03, c2 = 2.059601750e-04, c3 = 0.8615484887e-07;

double Thermistor(float Volts) {
   // Lecture brute (0–1023)
  double voltage = (Volts / 1023.0) * 3.3; // Conversion en tension
  double R_therm = R_FIXED * (3.3 / voltage - 1.0); // Calcul résistance thermistance

  // Formule Beta : 1/T = 1/T0 + (1/BETA)*ln(R/R0)
  double tempK = 1.0 / ( (1.0 / T0) + (1.0 / BETA) * log(R_therm / R0) );
  T = tempK - 273.15; // Conversion Kelvin -> Celsius
  return T;
}

void start(){
    if (tempLastStart==0){
      status = "working";
      V = Vo;
      tempLastStart = millis();
      if (tempLastStart==0) tempLastStart = 1;
    }
}

void stop(){
    status = "stopped";
    V = 0;
    tempLastStart = 0;
    ifttt();
}

void initHotend(){
  myPID.SetTunings(Kp, Ki, Kd);
  myPID.SetOutputLimits(0,Max);
  pinMode(LED_BUILTIN , OUTPUT);
  pinMode(PIN_FILAMENT , INPUT);
  if (status=="") start();
}


void hotendReadTempTask() {
  if (status == "stopped" && myPID.GetMode() == AUTOMATIC){
    myPID.SetMode(MANUAL);
    Output = 0;
  }
  if (status == "working" && myPID.GetMode() != AUTOMATIC){
    myPID.SetMode(AUTOMATIC);
  }
  if (millis() >= tempLastSample + 100)
  {
    Thermistor(analogRead(PIN_THERMISTOR)); //Volt to temp, update T
    if (T > Tm || isnan(T)){
      Output = 0;
    } else {
      myPID.Compute();
    }
    if (status == "working"){
      start();
      if (T > 150 || T > To + 20 ) {
        digitalWrite(LED_BUILTIN , LOW);// target temperature ready
      } else {
        digitalWrite(LED_BUILTIN , !digitalRead(LED_BUILTIN));//reaching tarjet temp
      }
    } else {
        digitalWrite(LED_BUILTIN , HIGH);
    }

    analogWrite(PIN_HEATER, Output);
    
    Fc = digitalRead(PIN_FILAMENT);
    
    if (Fc && !F) {
      tempLastFilament = millis();
      start();
    }
    
    if (!Fc && F) {
      tempLastFilament = 0;
      tempLastNoFilament = millis();
    }

    F = Fc;

    if (Fc && tempLastFilament > 0 && millis() >= tempLastFilament + 3*1000){
      Fi = true;
    }
    
    if (!Fc && Fi && tempLastNoFilament > 0 && millis() >= tempLastNoFilament + 500) { // no filament
      stop();
      tempLastNoFilament = 0;
      Fi = false;
    }
    
    if (!Fc && !Fi && tempLastStart > 0 && millis() >= tempLastStart + 5*60*1000) { // no filament for 5 min
      stop();
    }

    tempLastSample = millis();
    
  }
}
