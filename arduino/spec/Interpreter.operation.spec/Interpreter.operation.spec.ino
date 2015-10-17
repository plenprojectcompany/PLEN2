#line 2 "Interpreter.operation.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include "JointController.h"
#include "MotionController.h"
#include "Interpreter.h"


// ファイル内グローバルインスタンスの定義
namespace
{
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
	PLEN2::Interpreter      interpreter(motion_ctrl);

	PLEN2::Interpreter::Code code_obj;
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");

	joint_ctrl.loadSettings();

	code_obj.slot = 0;
	code_obj.loop_count = 0;
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

				if (interpreter.ready())
				{
					interpreter.popCode();
				}
			}
		}
	}

	if (Serial.available())
	{
		switch (Serial.read())
		{
			case 'i':
			{
				interpreter.pushCode(code_obj);

				break;
			}

			case 'o':
			{
				interpreter.popCode();

				break;
			}
		}
	}
}
