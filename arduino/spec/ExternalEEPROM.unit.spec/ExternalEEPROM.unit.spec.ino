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
    const uint16_t getRandomSlot()
    {
        using namespace PLEN2;

        return random(ExternalEEPROM::SLOT_BEGIN, ExternalEEPROM::SLOT_END);
    }

    void bufferRandomize(uint8_t data[], uint8_t size)
    {
        while (size--)
        {
            data[size] = random(0x00, 0xFF);
        }
    }

    bool checkIdentity(const uint8_t lhs[], const uint8_t rhs[], uint8_t size)
    {
        while (size--)
        {
            if (lhs[size] != rhs[size]) return false;
        }

        return true;
    }
}

/*!
    @brief ランダムに選択したスロットに読み書きするテスト

    ランダムな値を格納した配列を書き込んだ後、即座に読み込み同一性を検証します。
*/
test(RandomSlot_ReadWrite) 
{
    enum { BUFFER_SIZE = 30 };

    // Setup ===================================================================
    const uint16_t SLOT = getRandomSlot();

    uint8_t expected[BUFFER_SIZE] = { 0 };
    uint8_t actual[BUFFER_SIZE]   = { 0 };

    bufferRandomize(expected, BUFFER_SIZE);

    // Run =====================================================================
    PLEN2::ExternalEEPROM::writeSlot(SLOT, expected, BUFFER_SIZE);
    PLEN2::ExternalEEPROM::readSlot(SLOT, actual, BUFFER_SIZE);

    // Assert ==================================================================
    assertTrue( checkIdentity(expected, actual, BUFFER_SIZE) );
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

        for (uint16_t slot = PLEN2::ExternalEEPROM::SLOT_BEGIN;
             slot < PLEN2::ExternalEEPROM::SLOT_END;
             slot++
        )
        {
            // Setup ===========================================================
            uint8_t expected[BUFFER_SIZE] = { 0 };
            uint8_t actual[BUFFER_SIZE]   = { 0 };

            bufferRandomize(expected, BUFFER_SIZE);

            // Run =============================================================
            PLEN2::ExternalEEPROM::writeSlot(slot, expected, BUFFER_SIZE);
            PLEN2::ExternalEEPROM::readSlot(slot, actual, BUFFER_SIZE);

            // Assert ==========================================================
            assertTrue( checkIdentity(expected, actual, BUFFER_SIZE) );
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
    const uint16_t SLOT = getRandomSlot();

    uint8_t data[BUFFER_SIZE] = { 0 };

    // Run =====================================================================
    int8_t expected = -1;
    int8_t actual   = PLEN2::ExternalEEPROM::readSlot(SLOT, data, BUFFER_SIZE);

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
    const uint16_t SLOT = getRandomSlot();

    uint8_t data[BUFFER_SIZE] = { 0 };

    // Run =====================================================================
    int8_t expected = -1;
    int8_t actual   = PLEN2::ExternalEEPROM::writeSlot(SLOT, data, BUFFER_SIZE);

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
    const uint16_t SLOT = PLEN2::ExternalEEPROM::SLOT_END;

    uint8_t data[BUFFER_SIZE] = { 0 };

    // Run =====================================================================
    int8_t expected = -1;
    int8_t actual   = PLEN2::ExternalEEPROM::readSlot(SLOT, data, BUFFER_SIZE);

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
    const uint16_t SLOT = PLEN2::ExternalEEPROM::SLOT_END;

    uint8_t data[BUFFER_SIZE] = { 0 };

    // Run =====================================================================
    int8_t expected = -1;
    int8_t actual   = PLEN2::ExternalEEPROM::writeSlot(SLOT, data, BUFFER_SIZE);

    // Assert ==================================================================
    assertEqual(expected, actual);
}


/*!
    @brief アプリケーション・エントリポイント
*/
void setup()
{
    randomSeed( analogRead(PLEN2::Pin::RANDOM_DEVICE_IN) );

    PLEN2::ExternalEEPROM::begin();

    while (!Serial); // for the Arduino Leonardo/Micro only.

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    Test::run();
}
