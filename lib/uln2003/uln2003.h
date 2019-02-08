#ifndef _ULN2003_h_
#define _ULN2003_h_

#include <inttypes.h>
#include <Wire.h>




class ULN2003_StepperMotor {
 public:
  ULN2003_StepperMotor(void);




 private:

  void Stepper();
  void SetDirection() ;
  void WriteStep(uint8_t outArray[4]);
  
  uint8_t pinVector[4];
  uint16_t revsteps; // # steps per revolution
  uint8_t currentstep;
  uint8_t steppernum;
  String stepperName;

  boolean clockwise = true;
  uint8_t step = 0;
  uint8_t arrayDefault[4] = {LOW, LOW, LOW, LOW};
  uint8_t stepsMatrix[8][4] = {
    {LOW, LOW, LOW, HIGH},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, HIGH, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, HIGH, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {HIGH, LOW, LOW, LOW},
    {HIGH, LOW, LOW, HIGH},
  };
};
#endif
