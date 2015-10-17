#line 2 "Soul.operation.spec.ino"


// Arduinoライブラリ
#include <EEPROM.h>
#include <Wire.h>

// 独自ライブラリ
#include "Pin.h"
#include "AccelerationGyroSensor.h"
#include "JointController.h"
#include "MotionController.h"
#include "Soul.h"
#include "System.h"


namespace
{
	PLEN2::AccelerationGyroSensor sensor;
	PLEN2::JointController        joint_ctrl;
	PLEN2::MotionController       motion_ctrl(joint_ctrl);
	PLEN2::Soul                   soul(sensor, motion_ctrl);
	PLEN2::System                 system;
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	randomSeed(analogRead(PLEN2::Pin::RANDOM_DEVCIE_IN()));

	joint_ctrl.loadSettings();

	delay(3000);
}

void loop()
{
	if (motion_ctrl.playing())
	{
		if (motion_ctrl.frameUpdatable())
		{
			motion_ctrl.frameUpdate();
		}

		if (motion_ctrl.frameUpdateFinished())
		{
			if (motion_ctrl.nextFrameLoadable())
			{
				motion_ctrl.loadNextFrame();
			}
			else
			{
				motion_ctrl.stop();
			}
		}
	}

	if (system.USBSerial().available()) system.USBSerial().read();
	if (system.BLESerial().available()) system.BLESerial().read();

	soul.logging();
	soul.action();
}
