#line 2 "MotionController.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"
#include "MotionController.h"

// テストケース選択用プリプロセスマクロ
#define _TEST_USER false //!< ユーザテストについても実行します。
#define _TEST_HARD true //!< プロセッサに負荷のかかるテストについても実行します。

// ファイル内グローバルインスタンスの定義
namespace
{
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
}


/*!
	@brief ランダムに選択したスロットへの、モーションヘッダの設定テスト
*/
test(RandomSlot_SetHeader)
{
	const char SLOT = random(
		PLEN2::MotionController::Header::SLOT_BEGIN(),
		PLEN2::MotionController::Header::SLOT_END()
	);


	PLEN2::MotionController::Header expected;
	expected.slot = SLOT;
	for (
		int index = 0;
		index < PLEN2::MotionController::Header::NAME_LENGTH();
		index++
	)
	{
		expected.name[index] = random();
	}
	expected.frame_length = random(PLEN2::MotionController::Header::FRAMELENGTH_MIN(), PLEN2::MotionController::Header::FRAMELENGTH_MAX());
	expected.non_reserved_func_flags = random();
	expected.use_extra = random();
	expected.use_jump = random();
	expected.use_loop = random();
	expected.loop_begin = random();
	expected.loop_end = random();
	expected.loop_count = random();
	expected.jump_slot = random();
	expected.stop_flags[0] = random();
	expected.stop_flags[1] = random();
	expected.stop_flags[2] = random();
	motion_ctrl.setHeader(&expected);


	PLEN2::MotionController::Header actual;
	actual.slot = SLOT;
	motion_ctrl.getHeader(&actual);


	const char* p_expected = (const char*)&expected;
	const char* p_actual   = (const char*)&actual;
	for (
		int index = 0;
		index < sizeof(PLEN2::MotionController::Header);
		index++
	)
	{
		assertEqual(p_expected[index], p_actual[index]);
	}
}

/*!
	@brief 全てのスロットへの、モーションヘッダの設定テスト
*/
test(AllSlot_SetHeader)
{
	#if _TEST_HARD
		for (
			int slot = PLEN2::MotionController::Header::SLOT_BEGIN();
			slot < PLEN2::MotionController::Header::SLOT_END();
			slot++
		)
		{
			PLEN2::MotionController::Header expected;
			expected.slot = slot;
			for (
				int index = 0;
				index < PLEN2::MotionController::Header::NAME_LENGTH();
				index++
			)
			{
				expected.name[index] = random();
			}
			expected.frame_length = random(PLEN2::MotionController::Header::FRAMELENGTH_MIN(), PLEN2::MotionController::Header::FRAMELENGTH_MAX());
			expected.non_reserved_func_flags = random();
			expected.use_extra = random();
			expected.use_jump = random();
			expected.use_loop = random();
			expected.loop_begin = random();
			expected.loop_end = random();
			expected.loop_count = random();
			expected.jump_slot = random();
			expected.stop_flags[0] = random();
			expected.stop_flags[1] = random();
			expected.stop_flags[2] = random();
			motion_ctrl.setHeader(&expected);


			PLEN2::MotionController::Header actual;
			actual.slot = slot;
			motion_ctrl.getHeader(&actual);


			const char* p_expected = (const char*)&expected;
			const char* p_actual   = (const char*)&actual;
			for (
				int index = 0;
				index < sizeof(PLEN2::MotionController::Header);
				index++
			)
			{
				assertEqual(p_expected[index], p_actual[index]);
			}
		}
	#else
		skip();
	#endif
}

/*!
	@brief ランダムに選択したスロットへの、モーションフレームの設定テスト
*/
test(RandomSlotRandomFrame_SetFrame)
{
	const char SLOT = random(
		PLEN2::MotionController::Header::SLOT_BEGIN(),
		PLEN2::MotionController::Header::SLOT_END()
	);

	const char INDEX = random(PLEN2::MotionController::Frame::FRAME_END());


	PLEN2::MotionController::Frame expected;
	expected.index = INDEX;
	expected.transition_time_ms = random();
	for (
		int index = 0;
		index < PLEN2::JointController::SUM();
		index++
	)
	{
		expected.joint_angle[index] = random();
	}
	for (int index = 0; index < 8; index++)
	{
		expected.device_value[index] = random();
	}
	motion_ctrl.setFrame(SLOT, &expected);


	PLEN2::MotionController::Frame actual;
	actual.index = INDEX;
	motion_ctrl.getFrame(SLOT, &actual);


	const char* p_expected = (const char*)&expected;
	const char* p_actual   = (const char*)&actual;
	for (
		int index = 0;
		index < sizeof(PLEN2::MotionController::Frame);
		index++
	)
	{
		assertEqual(p_expected[index], p_actual[index]);
	}
}

/*!
	@brief 全てのスロットへの、モーションフレームの設定テスト
*/
test(AllSlotRandomFrame_SetFrame)
{
	#if _TEST_HARD
		const char INDEX = random(PLEN2::MotionController::Frame::FRAME_END());


		for (
			int slot = PLEN2::MotionController::Header::SLOT_BEGIN();
			slot < (PLEN2::MotionController::Header::SLOT_END());
			slot++
		)
		{
			PLEN2::MotionController::Frame expected;
			expected.index = INDEX;
			expected.transition_time_ms = random();
			for (
				int index = 0;
				index < PLEN2::JointController::SUM();
				index++
			)
			{
				expected.joint_angle[index] = random();
			}
			for (int index = 0; index < 8; index++)
			{
				expected.device_value[index] = random();
			}
			motion_ctrl.setFrame(slot, &expected);


			PLEN2::MotionController::Frame actual;
			actual.index =INDEX;
			motion_ctrl.getFrame(slot, &actual);


			const char* p_expected = (const char*)&expected;
			const char* p_actual   = (const char*)&actual;
			for (
				int index = 0;
				index < sizeof(PLEN2::MotionController::Frame);
				index++
			)
			{
				assertEqual(p_expected[index], p_actual[index]);
			}
		}
	#else
		skip();
	#endif
}

/*!
	@brief 未定義スロットへの、各種メソッドのテスト
*/
test(SlotOverflow_Methods)
{
	unsigned char SLOT = PLEN2::MotionController::Header::SLOT_END();
	PLEN2::MotionController::Header header;
	header.slot = SLOT;
	header.frame_length = 1;
	PLEN2::MotionController::Frame frame;
	frame.index = 0;


	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.getHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.setFrame(SLOT, &frame);
	assertEqual(expected, actual);

	actual = motion_ctrl.getFrame(SLOT, &frame);
	assertEqual(expected, actual);
}

/*!
	@brief 異常フレーム数に対する、各種メソッドのテスト
*/
test(FrameLengthOverflow_Methods)
{
	unsigned char FRAMELENGTH = PLEN2::MotionController::Header::FRAMELENGTH_MAX() + 1;
	PLEN2::MotionController::Header header;
	header.frame_length = FRAMELENGTH;
	header.slot = 0;
	PLEN2::MotionController::Frame frame;
	frame.index = FRAMELENGTH;


	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.setFrame(0, &frame);
	assertEqual(expected, actual);

	actual = motion_ctrl.getFrame(0, &frame);
	assertEqual(expected, actual);
}

/*!
	@brief 異常フレーム数に対する、各種メソッドのテスト
*/
test(FrameLengthUnderflow_Methods)
{
	unsigned char FRAMELENGTH = PLEN2::MotionController::Header::FRAMELENGTH_MIN() - 1;
	PLEN2::MotionController::Header header;
	header.frame_length = FRAMELENGTH;
	header.slot = 0;


	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);
}

/*!
	@brief ランダムに選択したスロットのダンプテスト

	ユーザによる目視でのテストです。
*/
test(RandomSlot_Dump)
{
	#if _TEST_USER
		const char SLOT = random(
			PLEN2::MotionController::Header::SLOT_BEGIN(),
			PLEN2::MotionController::Header::SLOT_END()
		);

		motion_ctrl.dump(SLOT);

		pass();
	#else
		skip();
	#endif
}

/*!
	@brief 全てのスロットのダンプテスト

	ユーザによる目視でのテストです。
*/
test(AllSlot_Dump)
{
	#if _TEST_USER && _TEST_HARD
		for (
			int slot = PLEN2::MotionController::Header::SLOT_BEGIN();
			slot < (PLEN2::MotionController::Header::SLOT_END());
			slot++
		)
		{
			motion_ctrl.dump(slot);
		}

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
