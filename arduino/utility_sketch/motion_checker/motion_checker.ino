#include <stdlib.h>
#include <string.h>

#include <Wire.h>
#include <EEPROM.h>
#include "System.h"
#include "JointController.h"
#include "MotionController.h"


namespace
{
	PLEN2::System           system;
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);

	const char BASE = 16;
}

namespace Purser
{
	char buffer[6] = { '\0', '\0', '\0', '\0', '\0', '\0' };
	char position  = 0;
}


/*!
	@brief Application Entry Point
*/
void setup()
{
	joint_ctrl.loadSettings();
	// joint_ctrl.resetSettings();
}

void loop()
{
loop_begin:

	if (motion_ctrl.playing())
	{
		if (motion_ctrl.frameUpdatable())
		{
			system.USBSerial().println("update : ");

			motion_ctrl.frameUpdate();
		}

		if (motion_ctrl.frameUpdateFinished())
		{
			system.USBSerial().println("update finished");

			if (motion_ctrl.nextFrameLoadable())
			{
				system.USBSerial().println("load next");

				motion_ctrl.loadNextFrame();
			}
			else
			{
				system.USBSerial().println("stop");

				motion_ctrl.stop();
			}
		}
	}

	if (system.USBSerial().available())
	{
		if (Purser::position == 0)
		{
			if (system.USBSerial().peek() != '$')
			{
				system.USBSerial().read();

				goto loop_begin;
			}
		}

		Purser::buffer[Purser::position++] = system.USBSerial().read();

		if (Purser::position == 5)
		{
			if (strncasecmp(Purser::buffer, "$mp", 3) == 0)
			{
				motion_ctrl.dump(atoi(Purser::buffer + 3));
				// motion_ctrl.play(strtol(Purser::buffer + 3, NULL, BASE));
			}
			
			Purser::position = 0;
		}
	}
}
