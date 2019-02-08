#ifdef USES_P048
//#######################################################################################################
//#################################### Plugin 048: Adafruit Motorshield v2 ##############################
//#######################################################################################################

// Adafruit Motorshield v2
// like this one: https://www.adafruit.com/products/1438
// based on this library: https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// written by https://github.com/krikk
// Currently DC Motors and Steppers are implemented, Servos are in default firmware!!!


#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>

#define PLUGIN_048
#define PLUGIN_ID_048         48
#define PLUGIN_NAME_048       "Motor - Adafruit Motorshield v2 [TESTING]"
#define PLUGIN_VALUENAME1_048 "MotorShield v2"

uint8_t Plugin_048_MotorShield_address = 0x60;

int Plugin_048_MotorStepsPerRevolution = 200;
int Plugin_048_StepperSpeed = 10;

Adafruit_MotorShield AFMS;
Adafruit_StepperMotor *myAFStepper[2];
AccelStepper *myAstepper[2];
int stepType[2];

void forwardstepM1();
void backwardstepM1();
void forwardstepM2();
void backwardstepM2();

boolean Plugin_048(byte function, struct EventStruct *event, String& string) {
	boolean success = false;


	switch (function) {

		case PLUGIN_DEVICE_ADD: {
			Device[++deviceCount].Number = PLUGIN_ID_048;
			Device[deviceCount].Type = DEVICE_TYPE_I2C;
			Device[deviceCount].VType = SENSOR_TYPE_NONE;
			Device[deviceCount].Ports = 0;
			Device[deviceCount].PullUpOption = false;
			Device[deviceCount].InverseLogicOption = false;
			Device[deviceCount].FormulaOption = true;
			Device[deviceCount].ValueCount = 2;
			Device[deviceCount].SendDataOption = false;
			Device[deviceCount].TimerOption = false;
			break;
		}

		case PLUGIN_GET_DEVICENAME: {
			string = F(PLUGIN_NAME_048);
			break;
		}

		case PLUGIN_GET_DEVICEVALUENAMES: {
			strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0],
					PSTR(PLUGIN_VALUENAME1_048));
			break;
		}

		case PLUGIN_WEBFORM_LOAD: {

    	addFormTextBox(F("I2C Address (Hex)"), F("p048_adr"),
		               formatToHex_decimal(PCONFIG(0)), 4);

    	addFormNumericBox(F("Stepper: steps per revolution"), F("p048_MotorStepsPerRevolution")
    			, PCONFIG(1));

    	addFormNumericBox(F("Stepper speed (rpm)"), F("p048_StepperSpeed")
    			, PCONFIG(2));

			success = true;
			break;
		}

		case PLUGIN_WEBFORM_SAVE: {
			String plugin1 = WebServer.arg(F("p048_adr"));
			PCONFIG(0) = (int) strtol(plugin1.c_str(), 0, 16);

			PCONFIG(1) = getFormItemInt(F("p048_MotorStepsPerRevolution"));

			PCONFIG(2) = getFormItemInt(F("p048_StepperSpeed"));
			success = true;
			break;
		}

		case PLUGIN_INIT: {
			Plugin_048_MotorShield_address = PCONFIG(0);
			Plugin_048_MotorStepsPerRevolution = PCONFIG(1);
			Plugin_048_StepperSpeed = PCONFIG(2);
			AFMS = Adafruit_MotorShield(Plugin_048_MotorShield_address);
			AFMS.begin();
			myAFStepper[0] = AFMS.getStepper(Plugin_048_MotorStepsPerRevolution, 1);
			myAFStepper[1] = AFMS.getStepper(Plugin_048_MotorStepsPerRevolution, 2);

			myAstepper[0] = (AccelStepper*) new AccelStepper(forwardstepM1, backwardstepM1);
			myAstepper[1] = (AccelStepper*) new AccelStepper(forwardstepM2, backwardstepM2);

			stepType[0]=SINGLE;
			stepType[1]=SINGLE;

			success = true;
			break;
		}


		case PLUGIN_WRITE: {
			String cmd = parseString(string, 1);

			// Commands:
			// MotorShieldCMD,<DCMotor>,<Motornumber>,<Forward/Backward/Release>,<Speed>
			// MotorShieldCMD,<Stepper>,<Motornumber>,<Forward/Backward/Release>,<Steps>,<SINGLE/DOUBLE/INTERLEAVE/MICROSTEP>

			if (cmd.equalsIgnoreCase(F("MotorShieldCMD")))
			{
        String param1 = parseString(string, 2);
        String param2 = parseString(string, 3);
        String param3 = parseString(string, 4);
        String param4 = parseString(string, 5);
        String param5 = parseString(string, 6);

				int p2_int;
				int p4_int;
				const bool param2_is_int = validIntFromString(param2, p2_int);
				const bool param4_is_int = validIntFromString(param4, p4_int);
				bool validRequest = true;

				String log = F("MotorShield: Address: 0x");
				log += String(Plugin_048_MotorShield_address,HEX);
				addLog(LOG_LEVEL_DEBUG, log);

				// set direction
				int direction = 0;
				if (param3.equalsIgnoreCase(F("Forward")))
					direction = FORWARD;
				else if (param3.equalsIgnoreCase(F("Backward")))
					direction = BACKWARD;
				else if (param3.equalsIgnoreCase(F("Release")))
					direction = RELEASE;
				else
					validRequest = false;


				// MotorShieldCMD,<DCMotor>,<Motornumber>,<Forward/Backward/Release>,<Speed>
				if (param1.equalsIgnoreCase(F("DCMotor"))) {
					if (param2_is_int && p2_int > 0 && p2_int < 5 && validRequest)
					{
						Adafruit_DCMotor *myMotor;
						myMotor = AFMS.getMotor(p2_int);
						byte speed = 255;
						if (param4_is_int && p4_int >= 0 && p4_int <= 255)
							speed = p4_int;
						myMotor->setSpeed(speed);
						myMotor->run(direction);
						success = true;
					}
				}

				// MotorShieldCMD,<Stepper>,<Motornumber>,<Forward/Backward/Release>,<Steps>,<SINGLE/DOUBLE/INTERLEAVE/MICROSTEP>
				if (param1.equalsIgnoreCase(F("Stepper")))
				{
					// Stepper# is which port it is connected to. If you're using M1 and M2, its port 1.
					// If you're using M3 and M4 indicate port 2
					if (param2_is_int && p2_int > 0 && p2_int < 3)
					{
						Adafruit_StepperMotor *myStepper =  myAFStepper[p2_int-1];
						myStepper->setSpeed(Plugin_048_StepperSpeed);
						AccelStepper *Astepper =  (AccelStepper *)myAstepper[p2_int-1];

						Astepper->setMaxSpeed(200);
						Astepper->setAcceleration(50);

						if (loglevelActiveFor(LOG_LEVEL_DEBUG_MORE)) {
							String log = F("MotorShield: StepsPerRevolution: ");
							log += String(Plugin_048_MotorStepsPerRevolution);
							log += F(" Stepperspeed: ");
							log += String(Plugin_048_StepperSpeed);
							addLog(LOG_LEVEL_DEBUG_MORE, log);
						}


						// set step Type
						if (param5.equalsIgnoreCase(F("SINGLE")))
							stepType[p2_int-1] = SINGLE;
						else if (param5.equalsIgnoreCase(F("DOUBLE")))
							stepType[p2_int-1] = DOUBLE;
						else if (param5.equalsIgnoreCase(F("INTERLEAVE")))
							stepType[p2_int-1] = INTERLEAVE;
						else if (param5.equalsIgnoreCase(F("MICROSTEP")))
							stepType[p2_int-1] = MICROSTEP;
						else	if (param5.equalsIgnoreCase(F("info")))
						{
							for (int i=1;i>=0;i--) {
								addLog(LOG_LEVEL_INFO, String(F("//////////////////////")));
								addLog(LOG_LEVEL_INFO, String(F("--- stepper info ")) + i + String(F(" --- ")));
								addLog(LOG_LEVEL_INFO, String(F("   current position:"))	+ myAstepper[i]->currentPosition());
								addLog(LOG_LEVEL_INFO, String(F("   target position :"))	+ myAstepper[i]->targetPosition());
								addLog(LOG_LEVEL_INFO, String(F("   is running      :"))	+ myAstepper[i]->isRunning());
								addLog(LOG_LEVEL_INFO, String(F("   distance to     :"))	+ myAstepper[i]->distanceToGo());
								addLog(LOG_LEVEL_INFO, String(F("   speed           :"))	+ myAstepper[i]->speed());
								addLog(LOG_LEVEL_INFO, String(F("   max speed       :"))	+ myAstepper[i]->maxSpeed());
								addLog(LOG_LEVEL_INFO, String(F("//////////////////////")));
							}
						} else
							validRequest = false;

						if (param4_is_int && p4_int != 0 && validRequest)
						{
							if (direction == RELEASE){
								addLog(LOG_LEVEL_INFO, String(F("Stepper")) + param2 + String(F("->Release.")));
								Astepper->stop();
								myStepper->release();
							} else if (Astepper->isRunning () == false) {
								Astepper->setSpeed(400);
								Astepper->setAcceleration(50);
								Astepper->setMaxSpeed(600);
								Astepper->move(p4_int * ((direction == FORWARD)?1:-1) );
							}
							success = true;
						}
					}
				}
			}
			break;
		}

		case PLUGIN_FIFTY_PER_SECOND:
			{
				success = true;
				// if (myAstepper[0]->currentPosition() != Asteps[0]) for (int i = 0;i<3;i++) myAstepper[0]->run();
				// if (myAstepper[1]->distanceToGo() != 0) for (int i = 0;i<3;i++) myAstepper[1]->run();
				if (myAstepper[0]->distanceToGo() != 0)  myAstepper[0]->run();
				if (myAstepper[1]->distanceToGo() != 0)  myAstepper[1]->run();
				UserVar[event->BaseVarIndex] = myAstepper[0]->distanceToGo();			// motor 1
				UserVar[event->BaseVarIndex + 1] = myAstepper[1]->distanceToGo();  // motor 2


				break;
			}

		case PLUGIN_READ:
			{
				success = true;
				break;
			}
	}
	return success;
}

void forwardstepM1() {
  myAFStepper[0]->step(1, FORWARD, stepType[0]);
}
void backwardstepM1() {
  myAFStepper[0]->step(1, BACKWARD, stepType[0]);
}

void forwardstepM2() {
   myAFStepper[1]->step(1, FORWARD, stepType[1]);
}
void backwardstepM2() {
  myAFStepper[1]->step(1, BACKWARD, stepType[1]);
}

//stepper[1] = new AccelStepper(forwardstepM2, backwardstepM2); // use functions to step


#endif // USES_P048
