#include "Arduino.h"
#include <EEPROM.h>

#include "System.h"
#include "JointController.h"


namespace
{
	PLEN2::System          system;
	PLEN2::JointController joint_ctrl;

	unsigned int pwm = 512;
}


void setup()
{
	joint_ctrl.loadSettings();
	// joint_ctrl.resetSettings();
}

void loop()
{
	if (system.USBSerial().available())
	{
		switch (system.USBSerial().read())
		{
			case 'm':
			{
				if (pwm != 1023)
				{
					pwm++;
				}

				break;
			}

			case 'p':
			{
				if (pwm != 0)
				{
					pwm--;
				}

				break;
			}

			default:
			{
				// noop.

				break;
			}
		}

		system.USBSerial().print(F("output : "));
		system.USBSerial().print(1023 - pwm);
		system.USBSerial().print(F(" (internal = "));
		system.USBSerial().print(pwm);
		system.USBSerial().println(F(")"));

		joint_ctrl.m_pwms[0] = pwm;
	}
}
