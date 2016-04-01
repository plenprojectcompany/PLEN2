#include <Arduino.h>
#include <EEPROM.h>

#include "System.h"
#include "JointController.h"


namespace
{
	PLEN2::JointController joint_ctrl;

	unsigned int pwm = 512;
}


void setup()
{
	volatile PLEN2::System s;

	joint_ctrl.loadSettings();
}

void loop()
{
	using namespace PLEN2;

	if (System::USBSerial().available())
	{
		switch (System::USBSerial().read())
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

		System::USBSerial().print(F("output : "));
		System::USBSerial().print(1023 - pwm);
		System::USBSerial().print(F(" (internal = "));
		System::USBSerial().print(pwm);
		System::USBSerial().println(F(")"));

		joint_ctrl.m_pwms[0] = pwm;
	}
}
