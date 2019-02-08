#include "Arduino.h"
#include <Wire.h>
#include "ULN2003.h"
#
#if defined(ARDUINO_SAM_DUE)
 #define WIRE Wire1
#else
 #define WIRE Wire
#endif



ULN2003_StepperMotor::ULN2003_StepperMotor() {
}

void ULN2003_StepperMotor::WriteStep(uint8_t outArray[4]){
  digitalWrite(pinVector[0], outArray[0]);
  digitalWrite(pinVector[1], outArray[1]);
  digitalWrite(pinVector[2], outArray[2]);
  digitalWrite(pinVector[3], outArray[3]);
}

void ULN2003_StepperMotor::Stepper() {
  if ((step >= 0) && (step < 8)) {
    WriteStep(stepsMatrix[step]);
  } else {
    WriteStep(arrayDefault);
  }
  SetDirection();
}

void ULN2003_StepperMotor::SetDirection() {
  (clockwise == true) ? (step++) : (step--);

  if (step > 7) {
    step = 0;
  } else if (step < 0) {
    step = 7;
  }
}
