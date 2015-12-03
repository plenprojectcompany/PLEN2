#line 2 "ExternalEEPROM.unit.spec.ino"


#include <Wire.h>

#include <ArduinoUnit.h>
#include "Pin.h"
#include "ExternalEEPROM.h"


/*!
	@brief テストケース選択用プリプロセスマクロ
*/
#define TEST_HARD false //!< プロセッサに負荷のかかるテストについても実行します。


namespace
{
	volatile PLEN2::ExternalEEPROM ext_eeprom;
}


/*!
	@brief ランダムに選択したスロットに読み書きするテスト

	ランダムな値を格納した配列を書き込んだ後、即座に読み込み同一性を検証します。
*/
test(RandomSlot_ReadWrite) 
{
	enum { BUFFER_SIZE = 30 };

	// Setup ===================================================================
	const int slot = random(
		PLEN2::ExternalEEPROM::SLOT_BEGIN(), PLEN2::ExternalEEPROM::SLOT_END()
	);

	char expected[BUFFER_SIZE] = { 0 };
	char actual[BUFFER_SIZE]   = { 0 };

	for (int index = 0; index < BUFFER_SIZE; index++)
	{
		expected[index] = random(-128, 128);
	}

	// Run =====================================================================
	PLEN2::ExternalEEPROM::writeSlot(slot, expected, BUFFER_SIZE);
	PLEN2::ExternalEEPROM::readSlot(slot, actual, BUFFER_SIZE);

	// Assert ==================================================================
	for (int index = 0; index < BUFFER_SIZE; index++)
	{
		assertEqual(expected[index], actual[index]);
	}
}

/*!
	@brief すべてのスロットに読み書きするテスト

	ランダムな値を格納した配列を書き込んだ後、即座に読み込み同一性を検証します。

	@attention
	このテストの実行には約41秒の時間を要します。
	また、EEPROMの寿命を著しく縮めるため、普段は実行しないことをおすすめします。
*/
test(AllSlot_ReadWrite)
{
	#if TEST_HARD
		enum { BUFFER_SIZE = 30 };

		for (int slot = PLEN2::ExternalEEPROM::SLOT_BEGIN();
			 slot < PLEN2::ExternalEEPROM::SLOT_END();
			 slot++
		)
		{
			// Setup ===========================================================
			char expected[BUFFER_SIZE] = { 0 };
			char actual[BUFFER_SIZE]   = { 0 };

			for (int index = 0; index < BUFFER_SIZE; index++)
			{
				expected[index] = random(-128, 128);
			}

			// Run =============================================================
			PLEN2::ExternalEEPROM::writeSlot(slot, expected, BUFFER_SIZE);
			PLEN2::ExternalEEPROM::readSlot(slot, actual, BUFFER_SIZE);

			// Assert ==========================================================
			for (int index = 0; index < BUFFER_SIZE; index++)
			{
				assertEqual(expected[index], actual[index]);
			}
		}
	#else
		skip();
	#endif
}

/*!
	@brief 超過サイズバッファへの読み込みテスト
*/
test(RandomSlot_ReadOverflow)
{
	enum { BUFFER_SIZE = 31 };

	// Setup ===================================================================
	const int slot = random(
		PLEN2::ExternalEEPROM::SLOT_BEGIN(),
		PLEN2::ExternalEEPROM::SLOT_END()
	);

	char data[BUFFER_SIZE] = { 0 };

	// Run =====================================================================
	int expected = -1;
	int actual   = PLEN2::ExternalEEPROM::readSlot(slot, data, BUFFER_SIZE);

	// Assert ==================================================================
	assertEqual(expected, actual);
}

/*!
	@brief 超過サイズバッファの書き込みテスト
*/
test(RandomSlot_WriteOverflow)
{
	enum { BUFFER_SIZE = 31 };

	// Setup ===================================================================
	const int slot = random(
		PLEN2::ExternalEEPROM::SLOT_BEGIN(), PLEN2::ExternalEEPROM::SLOT_END()
	);

	char data[BUFFER_SIZE] = { 0 };

	// Run =====================================================================
	int expected = -1;
	int actual   = PLEN2::ExternalEEPROM::writeSlot(slot, data, BUFFER_SIZE);

	// Assert ==================================================================
	assertEqual(expected, actual);
}

/*!
	@brief 超過スロットの読み込みテスト
*/
test(SlotOverflow_Read)
{
	enum { BUFFER_SIZE = 1 };

	// Setup ===================================================================
	const int slot = PLEN2::ExternalEEPROM::SLOT_END();
	char data[BUFFER_SIZE] = { 0 };

	// Run =====================================================================
	int expected = -1;
	int actual   = PLEN2::ExternalEEPROM::readSlot(slot, data, BUFFER_SIZE);

	// Assert ==================================================================
	assertEqual(expected, actual);
}

/*!
	@brief 超過スロットへの書き込みテスト
*/
test(SlotOverflow_Write)
{
	enum { BUFFER_SIZE = 1 };

	// Setup ===================================================================
	const int slot = PLEN2::ExternalEEPROM::SLOT_END();
	char data[BUFFER_SIZE] = { 0 };

	// Run =====================================================================
	int expected = -1;
	int actual   = PLEN2::ExternalEEPROM::writeSlot(slot, data, BUFFER_SIZE);

	// Assert ==================================================================
	assertEqual(expected, actual);
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	randomSeed(
		analogRead(PLEN2::Pin::RANDOM_DEVICE_IN())
	);

	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.print(F("# Test : "));
	Serial.println(__FILE__);
}

void loop()
{
	Test::run();
}
