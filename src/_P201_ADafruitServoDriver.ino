#ifdef USES_P201
//#######################################################################################################
//#################################### Plugin PLUGIN_ID_201: Adafruit_PWMServoDrive ##############################
//#######################################################################################################


#include <Adafruit_PWMServoDriver.h>

#define PLUGIN_201
#define PLUGIN_ID_201         201
#define PLUGIN_NAME_201       "Adafruit PWM Servo Driver"
#define PLUGIN_VALUENAME1_201 "gerry test v1"
#define P201_MAX_PINS  15
#define P201_MIN_PWM 0
#define P201_MAX_PWM 4095
#define P201_MIN_FREQUENCY   23.0 // Min possible PWM cycle frequency
#define P201_MAX_FREQUENCY   1500.0 // Max possible PWM cycle frequency
#define P201_ADDRESS 0x40  // I2C address
#define P201_MAX_ADDRESS 0x7F

#define P201_NUMS_ADDRESS P201_MAX_ADDRESS - P201_ADDRESS



uint8_t Plugin_201_ServoDriver_address = 0x40;
uint16_t Plugin_201_freq = PCA9685_MAX_FREQUENCY;
uint16_t Plugin_201_range = PCA9685_MAX_PWM;
Adafruit_PWMServoDriver *pwm;

boolean Plugin_201(byte function, struct EventStruct *event, String& string) {
	boolean success = false;

	Adafruit_MotorShield AFMS;



	switch (function) {

		case PLUGIN_DEVICE_ADD: {
			Device[++deviceCount].Number = PLUGIN_ID_201;
			Device[deviceCount].Type = DEVICE_TYPE_I2C;
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
			string = F(PLUGIN_NAME_201);
			break;
		}

  		case PLUGIN_GET_DEVICEVALUENAMES: {
  			strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0],	PSTR(PLUGIN_VALUENAME1_201));
  			break;
  		}

      case PLUGIN_WEBFORM_LOAD:
      {

        int optionValues[P201_NUMS_ADDRESS];
        for (int i=0;i < P201_NUMS_ADDRESS; i++)
        {
          optionValues[i] = P201_ADDRESS + i;
        }
        addFormSelectorI2C(F("i2c_addr"), P201_NUMS_ADDRESS, optionValues, Plugin_201_ServoDriver_address);


        String freqString = F("Frequency (");
        freqString += P201_MIN_FREQUENCY;
        freqString += '-';
        freqString += P201_MAX_FREQUENCY;
        freqString += ')';
        addFormNumericBox(freqString, F("p201_freq"), Plugin_201_freq, P201_MIN_FREQUENCY, P201_MAX_FREQUENCY);
        String funitString = F("default ");
        funitString += P201_MAX_FREQUENCY;
        addUnit(funitString);

        addFormNumericBox(F("Range (1-10000)"), F("p201_range"), Plugin_201_range, 1, 10000);
        String runitString = F("default ");
        runitString += P201_MAX_PWM;
        addUnit(runitString);

        success = true;
        break;
    }

    case PLUGIN_WEBFORM_SAVE: {
      CONFIG_PORT = getFormItemInt(F("i2c_addr"));
      PCONFIG(1) = getFormItemInt(F("p201_freq"));
      PCONFIG(2) = getFormItemInt(F("p201_range"));

      if (!IS_INIT(initializeState, (CONFIG_PORT - P201_ADDRESS)))
      {
        // if (PCONFIG(0) != mode2)
        //   Plugin_022_writeRegister(address, P201_MODE2, PCONFIG(0));
        // if (PCONFIG(1) != freq)
        //   Plugin_022_Frequency(address, PCONFIG(1));
      }

      addLog(LOG_LEVEL_INFO,
           String(F("AFSERVO---"))
         + Plugin_201_range + String(F("---"))
         + Plugin_201_freq + String(F("---")));
      success = true;
      break;
    }

		case PLUGIN_INIT: {
      pwm = new Adafruit_PWMServoDriver();

      Plugin_201_freq = PCONFIG(1);
      Plugin_201_range = PCONFIG(2);

      pwm->begin();
      pwm->setPWMFreq(Plugin_201_freq);  // Analog servos run at ~60 Hz updates


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

			if (cmd.equalsIgnoreCase(F("AFSERVO")))
			{

				String log = F("AdafruitServoDriver: Address: 0x");
				log += String(Plugin_201_ServoDriver_address,HEX);
				addLog(LOG_LEVEL_DEBUG, log);
        String param1 = parseString(string, 2);
        String param2 = parseString(string, 3);
        int p1_int;
        int p2_int;

        const bool param1_is_int = validIntFromString(param1, p1_int);
        const bool param2_is_int = validIntFromString(param2, p2_int);

					// MotorShieldCMD,<Stepper>,<Motornumber>,<Forward/Backward/Release>,<Steps>,<SINGLE/DOUBLE/INTERLEAVE/MICROSTEP>

					if (param2_is_int && param2_is_int )
					{
						success = true;
            setServoPulse(p1_int, p2_int);
            addLog(LOG_LEVEL_INFO,
                 String(F("AFSERVO---"))
               + p1_int + String(F("---"))
               + p2_int + String(F("---"))
               + Plugin_201_range + String(F("---"))
               + Plugin_201_freq + String(F("---")));
					}  else
           success=false;


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


int pulseWidth(int angle)
{
    int pulse_wide, analog_value;
    pulse_wide = map(angle, 0, 180, P201_MIN_PWM, Plugin_201_range);
    analog_value = int(float(pulse_wide) / 1000000 * Plugin_201_freq * 4096 );
    //Serial.println(analog_value);
    return analog_value;
}
// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {

  pwm->setPWM(n, 0, pulse);

//   double pulselength;
//
//   pulselength = 1000000;   // 1,000,000 us per second
//   pulselength /= 60;   // 60 Hz
// //  Serial.print(pulselength); Serial.println(" us per period");
//   pulselength /= 4096;  // 12 bits of resolution
// //  Serial.print(pulselength); Serial.println(" us per bit");
//   pulse *= 1000000;  // convert to us
//   pulse /= pulselength;
//   Serial.println(pulse);
//   pwm.setPWM(n, 0, pulse);
}

#endif // USES_P201
