#ifdef USES_P200
//#######################################################################################################
//#################################### Plugin PLUGIN_ID_200: uln2003 ##############################
//#######################################################################################################

// uln2003

//#include <ULN2003.h>

#define PLUGIN_200
#define PLUGIN_ID_200         200
#define PLUGIN_NAME_200       "Motor - uln2003"
#define PLUGIN_VALUENAME1_200 "gerry test v2"

uint8_t Plugin_200_MotorShield_address = 0x60;

int Plugin_200_MotorStepsPerRevolution = 4095;
int Plugin_200_StepperSpeed = 400;
int Plugin_200_IN1 = 0;
int Plugin_200_IN2 = 0;
int Plugin_200_IN3 = 0;
int Plugin_200_IN4 = 0;


const int NBSTEPS = 4095;
const int STEPTIME = 1000;
int Step = 0;
boolean Clockwise = true;
unsigned long lastTime = 0L;

int arrayDefault[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[8][4] = {
  {LOW, LOW, LOW, HIGH},
  {LOW, LOW, HIGH, HIGH},
  {LOW, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, LOW},
  {HIGH, LOW, LOW, HIGH},
};

void writeStep(int outArray[4]);
void stepper();
void setDirection();
void moveStep();


boolean Plugin_200(byte function, struct EventStruct *event, String& string) {
	boolean success = false;

	Adafruit_MotorShield AFMS;



	switch (function) {

		case PLUGIN_DEVICE_ADD: {
			Device[++deviceCount].Number = PLUGIN_ID_200;
			Device[deviceCount].Type = DEVICE_TYPE_QUADRUPLE;
			Device[deviceCount].VType = SENSOR_TYPE_NONE;
			Device[deviceCount].Ports = 0;
			Device[deviceCount].PullUpOption = false;
			Device[deviceCount].InverseLogicOption = false;
			Device[deviceCount].FormulaOption = false;
			Device[deviceCount].ValueCount = 0;
			Device[deviceCount].SendDataOption = false;
			Device[deviceCount].TimerOption = false;
			break;
		}

		case PLUGIN_GET_DEVICENAME: {
			string = F(PLUGIN_NAME_200);
			break;
		}

		case PLUGIN_GET_DEVICEVALUENAMES: {
			strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0],
					PSTR(PLUGIN_VALUENAME1_200));
			break;
		}

		case PLUGIN_WEBFORM_LOAD: {


    	addFormNumericBox(F("Stepper: steps per revolution"), F("p200_MotorStepsPerRevolution"), PCONFIG(4));

    	addFormNumericBox(F("Stepper speed (rpm)"), F("p200_StepperSpeed"), PCONFIG(5));



			success = true;
			break;
		}

		case PLUGIN_WEBFORM_SAVE: {
      int8_t Plugin_013_Pin1 = CONFIG_PIN1;
      int8_t Plugin_013_Pin2 = CONFIG_PIN2;
      int8_t Plugin_013_Pin3 = CONFIG_PIN3;
      int8_t Plugin_013_Pin4 = CONFIG_PIN4;

			PCONFIG(4) = getFormItemInt(F("p200_MotorStepsPerRevolution"));

      PCONFIG(5) = getFormItemInt(F("p200_StepperSpeed"));
			success = true;
			break;
		}

		case PLUGIN_INIT: {

			Plugin_200_MotorStepsPerRevolution = PCONFIG(4);
      Plugin_200_StepperSpeed = PCONFIG(5);

      Plugin_200_IN1= CONFIG_PIN1;
      Plugin_200_IN2= CONFIG_PIN2;
      Plugin_200_IN3= CONFIG_PIN3;
      Plugin_200_IN4= CONFIG_PIN4;

			pinMode (Plugin_200_IN1,OUTPUT);
			pinMode (Plugin_200_IN2,OUTPUT);
			pinMode (Plugin_200_IN3,OUTPUT);
			pinMode (Plugin_200_IN4,OUTPUT);
			success = true;
			break;
		}

		case PLUGIN_READ: {

			success = false;
			break;
		}

		case PLUGIN_WRITE: {
			String cmd = parseString(string, 1);

			// Commands:
			// MotorShieldCMD,<DCMotor>,<Motornumber>,<Forward/Backward/Release>,<Speed>

			if (cmd.equalsIgnoreCase(F("stepper")))
			{
        String param1 = parseString(string, 2);
        String param2 = parseString(string, 3);


        int p2_int;

        const bool param2_is_int = validIntFromString(param2, p2_int);

        bool validRequest = true;
         addLog(LOG_LEVEL_INFO, String(F("STEPPER---"))
            + Plugin_200_IN1 + String(F("---"))
            + Plugin_200_IN2 + String(F("---"))
            + Plugin_200_IN3 + String(F("---"))
            + Plugin_200_IN4 + String(F("---")));


//				String param2 = parseString(string, 3);
//				String param3 = parseString(string, 4);
//
//				int p2_int;
//				int p3_int;
//				const bool param2_is_int = validIntFromString(param2, p2_int);
//			const bool param3_is_int = validfloatfromstring(param3, p3_int);



				String log = F("uln2003: Address: 0x");
				log += String(Plugin_200_MotorShield_address,HEX);
				addLog(LOG_LEVEL_DEBUG, log);

					// MotorShieldCMD,<Stepper>,<Motornumber>,<Forward/Backward/Release>,<Steps>,<SINGLE/DOUBLE/INTERLEAVE/MICROSTEP>

					if (param1.equalsIgnoreCase(F("F")))
					{
						Clockwise=true;
						moveStep();
						success = true;
					}

					if (param1.equalsIgnoreCase(F("B")))
					{
						Clockwise=false;
						moveStep();
						success = true;
					}

					if (param1.equalsIgnoreCase(F("stop")))
					{
						success = true;
					}

				}



			break;
		}

		case PLUGIN_TEN_PER_SECOND:
			{
				success = true;
				break;
			}


	}
	return success;
}

void moveStep() {
		unsigned long currentMicros;

		int stepsLeft = NBSTEPS;
    lastTime = micros();

	  while (stepsLeft > 0) {
	    currentMicros = micros();
	    if (currentMicros - lastTime >= STEPTIME) {
	      stepper();
	      lastTime = micros();
	      stepsLeft--;
	    }
			//yield();
		}
  	writeStep(arrayDefault);
}

void writeStep(int outArray[4]) {
  digitalWrite(Plugin_200_IN1, outArray[0]);
  digitalWrite(Plugin_200_IN2, outArray[1]);
  digitalWrite(Plugin_200_IN3, outArray[2]);
  digitalWrite(Plugin_200_IN4, outArray[3]);
}

void stepper() {
  if ((Step >= 0) && (Step < 8)) {
    writeStep(stepsMatrix[Step]);
  } else {
    writeStep(arrayDefault);
  }
  setDirection();
}

void setDirection() {
  (Clockwise == true) ? (Step++) : (Step--);

  if (Step > 7) {
    Step = 0;
  } else if (Step < 0) {
    Step = 7;
  }
}
#endif // USES_P200
