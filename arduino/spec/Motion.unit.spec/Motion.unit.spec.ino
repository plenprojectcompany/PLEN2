#line 2 "Motion.unit.spec.ino"


#include <EEPROM.h>
#include <Wire.h>

#include <ArduinoUnit.h>
#include "Pin.h"
#include "System.h"
#include "ExternalEEPROM.h"
#include "JointController.h"
#include "Motion.h"


/*!
	@brief テストケース選択用プリプロセスマクロ
*/
#define TEST_HARD false //!< プロセッサに負荷のかかるテストについても実行します。


namespace
{
	void validRandomize(PLEN2::Motion::Header& header)
	{
		using namespace PLEN2::Motion;

		header.frame_length  = random(Header::FRAMELENGTH_MIN, Header::FRAMELENGTH_MAX + 1);
		header.use_extra     = random();
		header.use_jump      = random();
		header.use_loop      = random();
		header.loop_begin    = random();
		header.loop_end      = random();
		header.loop_count    = random();
		header.jump_slot     = random();
		header.stop_flags[0] = random();
		header.stop_flags[1] = random();
		header.stop_flags[2] = random();

		for (int index = 0; index < Header::NAME_LENGTH; index++)
		{
			header.name[index] = random();
		}
	}

	void validRandomize(PLEN2::Motion::Frame& frame)
	{
		using namespace PLEN2;
		using namespace PLEN2::Motion;

		frame.transition_time_ms = random();

		for (int index = 0; index < JointController::SUM; index++)
		{
			frame.joint_angle[index] = random();
		}

		for (int index = 0; index < 8; index++)
		{
			frame.device_value[index] = random();
		}
	}
}


/*!
	@brief ランダムに選択したスロットへの、モーションヘッダの設定テスト
*/
test(RandomSlot_SetHeader)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	const char SLOT = random(SLOT_BEGIN, SLOT_END);

	Header expected, actual;

	validRandomize(expected);
	expected.slot = actual.slot = SLOT;

	// Run =====================================================================
	expected.set();
	actual.get();

	// Assert ==================================================================
	const char* expected_ptr = reinterpret_cast<const char*>(&expected);
	const char* actual_ptr   = reinterpret_cast<const char*>(&actual);

	for (int index = 0; index < sizeof(Header); index++)
	{
		assertEqual(expected_ptr[index], actual_ptr[index]);
	}
}


/*!
	@brief 全てのスロットへの、モーションヘッダの設定テスト
*/
test(AllSlot_SetHeader)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Header expected, actual;

	for (char slot = SLOT_BEGIN; slot < SLOT_END; slot++)
	{
		// Setup ===============================================================
		validRandomize(expected);
		expected.slot = actual.slot = slot;

		// Run =================================================================
		expected.set();
		actual.get();

		// Assert ==============================================================
		const char* expected_ptr = reinterpret_cast<const char*>(&expected);
		const char* actual_ptr   = reinterpret_cast<const char*>(&actual);

		for (int index = 0; index < sizeof(Header); index++)
		{
			assertEqual(expected_ptr[index], actual_ptr[index]);
		}
	}
}


/*!
	@brief ランダムに選択したスロットへの、モーションフレーム単体の設定テスト
*/
test(RandomSlotRandomFrame_SetFrame)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	const char SLOT  = random(SLOT_BEGIN, SLOT_END);
	const char INDEX = random(Frame::FRAME_BEGIN, Frame::FRAME_END);

	Frame expected, actual;

	validRandomize(expected);
	expected.index = actual.index = INDEX;

	// Run =====================================================================
	expected.set(SLOT);
	actual.get(SLOT);

	// Assert ==================================================================
	const char* expected_ptr = reinterpret_cast<const char*>(&expected);
	const char* actual_ptr   = reinterpret_cast<const char*>(&actual);

	for (int index = 0; index < sizeof(Frame); index++)
	{
		assertEqual(expected_ptr[index], actual_ptr[index]);
	}
}


/*!
	@brief ランダムに選択したスロットへの、モーションフレーム全体の設定テスト
*/
test(RandomSlotAllFrame_SetFrame)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	const char SLOT = random(SLOT_BEGIN, SLOT_END);

	Frame expected, actual;

	for (char index = Frame::FRAME_BEGIN; index < Frame::FRAME_END; index++)
	{
		// Setup ===============================================================
		validRandomize(expected);
		expected.index = actual.index = index;

		// Run =================================================================
		expected.set(SLOT);
		actual.get(SLOT);

		// Assert ==============================================================
		const char* expected_ptr = reinterpret_cast<const char*>(&expected);
		const char* actual_ptr   = reinterpret_cast<const char*>(&actual);

		for (int index = 0; index < sizeof(Frame); index++)
		{
			assertEqual(expected_ptr[index], actual_ptr[index]);
		}
	}
}


/*!
	@brief 全てのスロットへの、モーションフレーム全体の設定テスト
*/
test(AllSlotAllFrame_SetFrame)
{
	#if TEST_HARD

	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Frame expected, actual;

	for (char slot = SLOT_BEGIN; slot < SLOT_END; slot++)
	{
		for (char index = Frame::FRAME_BEGIN; index < Frame::FRAME_END; index++)
		{
			// Setup ===========================================================
			validRandomize(expected);
			expected.index = actual.index = index;

			// Run =============================================================
			expected.set(slot);
			actual.get(slot);

			// Assert ==========================================================
			const char* expected_ptr = reinterpret_cast<const char*>(&expected);
			const char* actual_ptr   = reinterpret_cast<const char*>(&actual);

			for (int index = 0; index < sizeof(Frame); index++)
			{
				assertEqual(expected_ptr[index], actual_ptr[index]);
			}
		}
	}

	#else
		skip();
	#endif
}


/*!
	@brief ヘッダ書き込みにおける、異常系のテスト
*/
test(SetHeader_InvalidInputs)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Header header;

	{
		// Setup ===============================================================
		header.slot = SLOT_END; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = header.set();

		// Assert ==============================================================
		assertEqual(expected, actual);
	}

	{
		// Setup ===============================================================
		header.slot         = 0;
		header.frame_length = 0; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = header.set();

		// Assert ==============================================================
		assertEqual(expected, actual);
	}

	{
		// Setup ===============================================================
		header.slot         = 0;
		header.frame_length = Header::FRAMELENGTH_MAX + 1; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = header.set();

		// Assert ==============================================================
		assertEqual(expected, actual);
	}
}


/*!
	@brief ヘッダ読み込みにおける、異常系のテスト
*/
test(GetHeader_InvalidInputs)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Header header;

	{
		// Setup ===============================================================
		header.slot = SLOT_END; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = header.get();

		// Assert ==============================================================
		assertEqual(expected, actual);
	}
}


/*!
	@brief フレーム書き込みにおける、異常系のテスト
*/
test(SetFrame_InvalidInputs)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Frame frame;

	{
		// Setup ===============================================================
		frame.index = 0;

		// Run =================================================================
		bool expected = false;
		bool actual   = frame.set(SLOT_END /* = Invalid */);

		// Assert ==============================================================
		assertEqual(expected, actual);
	}

	{
		// Setup ===============================================================
		frame.index = Frame::FRAME_END; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = frame.set(0);

		// Assert ==============================================================
		assertEqual(expected, actual);
	}
}


/*!
	@brief フレーム読み込みにおける、異常系のテスト
*/
test(GetFrame_InvalidInputs)
{
	using namespace PLEN2::Motion;

	// Setup ===================================================================
	Frame frame;

	{
		// Setup ===============================================================
		frame.index = 0;

		// Run =================================================================
		bool expected = false;
		bool actual   = frame.get(SLOT_END /* = Invalid */);

		// Assert ==============================================================
		assertEqual(expected, actual);
	}

	{
		// Setup ===============================================================
		frame.index = Frame::FRAME_END; // Invalid

		// Run =================================================================
		bool expected = false;
		bool actual   = frame.get(0);

		// Assert ==============================================================
		assertEqual(expected, actual);
	}
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	randomSeed(
		analogRead(PLEN2::Pin::RANDOM_DEVICE_IN())
	);

	volatile PLEN2::System         system;
	volatile PLEN2::ExternalEEPROM exteeprom;

	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.print(F("# Test : "));
	Serial.println(__FILE__);
}

void loop()
{
	Test::run();
}
