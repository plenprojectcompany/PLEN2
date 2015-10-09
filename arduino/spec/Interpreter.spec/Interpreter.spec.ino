#line 2 "Interpreter.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"
#include "MotionController.h"
#include "Interpreter.h"


// ファイル内グローバルインスタンスの定義
namespace
{
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
	PLEN2::Interpreter      interpreter(motion_ctrl);
}


/*!
	@brief push操作とpop操作の等試行確認テスト
*/
test(PushPop_Equals)
{
	int push_count = 0;
	int pop_count  = 0;

	PLEN2::Interpreter::Code code;
	interpreter.reset();
	while (interpreter.pushCode(code))
	{
		push_count++;
	}

	while (interpreter.popCode())
	{
		pop_count++;
	}

	assertEqual(push_count, pop_count);
}


/*!
	@brief push操作の限界試行テスト
*/
test(PushCode_Overflow)
{
	bool expected = false;

	PLEN2::Interpreter::Code code;
	interpreter.reset();
	bool actual = true;

	for (int index = 0; index < PLEN2::Interpreter::QUEUE_SIZE(); index++)
	{
		actual = interpreter.pushCode(code);
	}

	assertEqual(expected, actual);
}


/*!
	@brief 空のインタプリタに対するpop操作テスト
*/
test(PopCode_Empty)
{
	bool expected = false;

	interpreter.reset();
	bool actual = interpreter.popCode();

	assertEqual(expected, actual);
}


/*!
	@brief push操作によってready状態に移行するかのテスト
*/
test(Ready)
{
	bool expected = true;

	PLEN2::Interpreter::Code code;
	interpreter.reset();
	interpreter.pushCode(code);
	bool actual = interpreter.ready();

	assertEqual(expected, actual);
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
