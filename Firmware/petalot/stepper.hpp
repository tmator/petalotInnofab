#include <AccelStepper.h>

int V;

AccelStepper stepper(AccelStepper::FULL4WIRE,PIN_STEP1,PIN_STEP2,PIN_STEP3,PIN_STEP4);

int stepsPerRevolution = 200;  

bool stepperEnable = false;

void initStepper(){
  stepper.setPinsInverted(true,false,true); //set enable pin inverted
  //stepper.setEnablePin(PIN_EN);
  stepper.disableOutputs();
  stepper.setMaxSpeed(40*stepsPerRevolution+1);
}

void stepperRunTask(){
  if (status == "stopped" && stepperEnable) {
    stepper.disableOutputs();
    stepperEnable = false;
  }
  if (status == "working" && !stepperEnable) {
    stepper.enableOutputs();
    stepperEnable = true;
  }
  if (status == "working") {
    stepper.setSpeed(Vo*stepsPerRevolution);
    stepper.runSpeed();
  }
}
