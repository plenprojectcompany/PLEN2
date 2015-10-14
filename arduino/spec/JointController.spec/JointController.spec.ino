#line 2 "JointController.spec.ino"


// Arduinoライブラリ関連
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"


#define _TEST_USER false //!< ユーザテストについても実行します。

namespace {
	PLEN2::JointController joint_ctrl;

	unsigned int angle2PWM(unsigned char joint_id, int angle)
	{
		angle = constrain(angle, joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id));

		unsigned int pwm = map(
			angle,
			PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
			PLEN2::JointController::PWM_MIN(),   PLEN2::JointController::PWM_MAX()
		);

		return pwm;
	}

	unsigned int angleDiff2PWM(unsigned char joint_id, int angle_diff)
	{
		int angle = constrain(
			angle_diff + joint_ctrl.getHomeAngle(joint_id),
			joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id)
		);

		unsigned int pwm = map(
			angle,
			PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
			PLEN2::JointController::PWM_MIN(),   PLEN2::JointController::PWM_MAX()
		);

		return pwm;
	}
}


/*!
	@brief ランダムに選択した関節への、角度最小値の設定テスト
*/
test(RandomJoint_SetMinAngle)
{
	unsigned char joint_id = random(PLEN2::JointController::SUM());


	int expected = random(
		PLEN2::JointController::ANGLE_MIN(), joint_ctrl.getMaxAngle(joint_id)
	);


	joint_ctrl.setMinAngle(joint_id, expected);
	joint_ctrl.loadSettings();
	int actual = joint_ctrl.getMinAngle(joint_id);


	assertEqual(expected, actual);
}

/*!
	@brief 全ての関節への、角度最小値の設定テスト
*/
test(AllJoint_SetMinAngle)
{
	for (unsigned char joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		int expected = random(
			PLEN2::JointController::ANGLE_MIN(), joint_ctrl.getMaxAngle(joint_id)
		);


		joint_ctrl.setMinAngle(joint_id, expected);
		joint_ctrl.loadSettings();
		int actual = joint_ctrl.getMinAngle(joint_id);


		assertEqual(expected, actual);
	}
}

/*!
	@brief ランダムに選択した関節への、角度最大値の設定テスト
*/
test(RandomJoint_SetMaxAngle)
{
	unsigned char joint_id = random(PLEN2::JointController::SUM());


	int expected = random(
		joint_ctrl.getMinAngle(joint_id) + 1, PLEN2::JointController::ANGLE_MAX() + 1
	);


	joint_ctrl.setMaxAngle(joint_id, expected);
	joint_ctrl.loadSettings();
	int actual = joint_ctrl.getMaxAngle(joint_id);


	assertEqual(expected, actual);
}

/*!
	@brief 全ての関節への、角度最大値の設定テスト
*/
test(AllJoint_SetMaxAngle)
{
	for (unsigned char joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		int expected = random(
			joint_ctrl.getMinAngle(joint_id) + 1, PLEN2::JointController::ANGLE_MAX() + 1
		);


		joint_ctrl.setMaxAngle(joint_id, expected);
		joint_ctrl.loadSettings();
		int actual = joint_ctrl.getMaxAngle(joint_id);


		assertEqual(expected, actual);
	}
}

/*!
	@brief ランダムに選択した関節への、角度初期値の設定テスト
*/
test(RandomJoint_SetHomeAngle)
{
	unsigned char joint_id = random(PLEN2::JointController::SUM());


	int expected = random(
		joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id) + 1
	);


	joint_ctrl.setHomeAngle(joint_id, expected);
	joint_ctrl.loadSettings();
	int actual = joint_ctrl.getHomeAngle(joint_id);


	assertEqual(expected, actual);
}

/*!
	@brief 全ての関節への、角度初期値の設定テスト
*/
test(AllJoint_SetHomeAngle)
{
	for (unsigned char joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		int expected = random(
			joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id) + 1
		);


		joint_ctrl.setHomeAngle(joint_id, expected);
		joint_ctrl.loadSettings();
		int actual = joint_ctrl.getHomeAngle(joint_id);


		assertEqual(expected, actual);
	}
}

/*!
	@brief ランダムに選択した関節への、角度の設定テスト
*/
test(RandomJoint_SetAngle)
{
	unsigned char joint_id = random(PLEN2::JointController::SUM());
	int angle = random(
		joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id) + 1
	);


	unsigned int expected = angle2PWM(joint_id, angle);


	joint_ctrl.setAngle(joint_id, angle);
	unsigned int actual = PLEN2::JointController::m_pwms[joint_id];


	assertEqual(expected, actual);
}

/*!
	@brief 全ての関節への、角度の設定テスト
*/
test(AllJoint_SetAngle)
{
	for (unsigned char joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		int angle = random(
			joint_ctrl.getMinAngle(joint_id), joint_ctrl.getMaxAngle(joint_id) + 1
		);


		unsigned int expected = angle2PWM(joint_id, angle);


		joint_ctrl.setAngle(joint_id, angle);
		unsigned int actual = PLEN2::JointController::m_pwms[joint_id];


		assertEqual(expected, actual);
	}
}

/*!
	@brief ランダムに選択した関節への、角度差分の設定テスト
*/
test(RandomJoint_SetAngleDiff)
{
	unsigned char joint_id = random(PLEN2::JointController::SUM());
	
	int angle_diff = random(
		joint_ctrl.getMinAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id),
		joint_ctrl.getMaxAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id) + 1
	);


	unsigned int expected = angleDiff2PWM(joint_id, angle_diff);


	joint_ctrl.setAngleDiff(joint_id, angle_diff);
	unsigned int actual = PLEN2::JointController::m_pwms[joint_id];


	assertEqual(expected, actual);
}

/*!
	@brief 全ての関節への、角度差分の設定テスト
*/
test(AllJoint_SetAngleDiff)
{
	for (unsigned char joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		int angle_diff = random(
			joint_ctrl.getMinAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id),
			joint_ctrl.getMaxAngle(joint_id) - joint_ctrl.getHomeAngle(joint_id) + 1
		);


		unsigned int expected = angleDiff2PWM(joint_id, angle_diff);


		joint_ctrl.setAngleDiff(joint_id, angle_diff);
		unsigned int actual = PLEN2::JointController::m_pwms[joint_id];


		assertEqual(expected, actual);
	}
}

/*!
	@brief 未定義関節への、各種取得メソッドのテスト
*/
test(JointOverflow_GetMethods)
{
	unsigned char joint_id = PLEN2::JointController::SUM();

	int expected = -32768;
	int actual;

	actual = joint_ctrl.getMinAngle(joint_id);
	assertEqual(expected, actual);

	actual = joint_ctrl.getMaxAngle(joint_id);
	assertEqual(expected, actual);

	actual = joint_ctrl.getHomeAngle(joint_id);
	assertEqual(expected, actual);
}

/*!
	@brief 未定義関節への、各種設定メソッドのテスト
*/
test(JointOverflow_SetMethods)
{
	unsigned char joint_id = PLEN2::JointController::SUM();
	unsigned int  angle    = random();

	bool expected = false;
	bool actual;

	actual = joint_ctrl.setMinAngle(joint_id, angle);
	assertEqual(expected, actual);

	actual = joint_ctrl.setMaxAngle(joint_id, angle);
	assertEqual(expected, actual);

	actual = joint_ctrl.setHomeAngle(joint_id, angle);
	assertEqual(expected, actual);

	actual = joint_ctrl.setAngle(joint_id, angle);
	assertEqual(expected, actual);

	actual = joint_ctrl.setAngleDiff(joint_id, angle);
	assertEqual(expected, actual);
}

/*!
	@brief タイマ1の動作テスト
*/
test(Timer1Attached)
{
	unsigned char before = joint_ctrl.m_overflow_count;


	unsigned long end = millis() + 1000;
	while (millis() < end);
	unsigned char after = joint_ctrl.m_overflow_count;


	assertNotEqual(before, after);
}

/*!
	@brief 関節設定のダンプテスト

	ユーザによる目視でのテストです。
*/
test(Dump)
{
	#if _TEST_USER
		joint_ctrl.dump();

		pass();
	#else
		skip();
	#endif
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	randomSeed(analogRead(0));
	joint_ctrl.loadSettings();
	joint_ctrl.resetSettings();

	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
