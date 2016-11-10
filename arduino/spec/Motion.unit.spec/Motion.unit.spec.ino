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
#define TEST_HARD true //!< プロセッサに負荷のかかるテストについても実行します。


namespace
{
    const uint8_t getRandomSlot()
    {
        using namespace PLEN2::Motion;

        return random(SLOT_BEGIN, SLOT_END);
    }

    const uint8_t getRandomIndex()
    {
        using namespace PLEN2::Motion;

        return random(Frame::FRAME_BEGIN, Frame::FRAME_END);
    }

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

        for (int index = 0; index < JointController::JOINTS_SUM; index++)
        {
            frame.joint_angle[index] = random();
        }
    }

    template<typename T>
    bool checkIdentity(const T& lhs, const T& rhs, uint8_t size)
    {
        const uint8_t* lhs_ptr = reinterpret_cast<const uint8_t*>(&lhs);
        const uint8_t* rhs_ptr = reinterpret_cast<const uint8_t*>(&rhs);

        while (size--)
        {
            if (lhs_ptr[size] != rhs_ptr[size]) return false;
        }

        return true;
    }
}


/*!
    @brief ランダムに選択したスロットへの、モーションヘッダの設定テスト
*/
test(RandomSlot_SetHeader)
{
    using namespace PLEN2::Motion;

    // Setup ==================================================================
    const uint8_t SLOT = getRandomSlot();

    Header expected, actual;

    validRandomize(expected);

    // Run ====================================================================
    Header::set(SLOT, expected);
    Header::get(SLOT, actual);

    // Assert =================================================================
    assertTrue( checkIdentity(expected, actual, sizeof(Header)) );
}


/*!
    @brief 全てのスロットへの、モーションヘッダの設定テスト
*/
test(AllSlot_SetHeader)
{
    using namespace PLEN2::Motion;

    // Setup ==================================================================
    Header expected, actual;

    for (uint8_t slot = SLOT_BEGIN; slot < SLOT_END; slot++)
    {
        // Setup ==============================================================
        validRandomize(expected);

        // Run ================================================================
        Header::set(slot, expected);
        Header::get(slot, actual);

        // Assert =============================================================
        assertTrue( checkIdentity(expected, actual, sizeof(Header)) );
    }
}


/*!
    @brief ランダムに選択したスロットへの、モーションフレーム単体の設定テスト
*/
test(RandomSlotRandomFrame_SetFrame)
{
    using namespace PLEN2::Motion;

    // Setup ==================================================================
    const char SLOT  = getRandomSlot();
    const char INDEX = getRandomIndex();

    Frame expected, actual;

    validRandomize(expected);

    // Run ====================================================================
    Frame::set(SLOT, INDEX, expected);
    Frame::get(SLOT, INDEX, actual);

    // Assert =================================================================
    assertTrue( checkIdentity(expected, actual, sizeof(Frame)) );
}


/*!
    @brief ランダムに選択したスロットへの、モーションフレーム全体の設定テスト
*/
test(RandomSlotAllFrame_SetFrame)
{
    using namespace PLEN2::Motion;

    // Setup ==================================================================
    const char SLOT = getRandomSlot();

    Frame expected, actual;

    for (uint8_t index = Frame::FRAME_BEGIN; index < Frame::FRAME_END; index++)
    {
        // Setup ==============================================================
        validRandomize(expected);

        // Run ================================================================
        Frame::set(SLOT, index, expected);
        Frame::get(SLOT, index, actual);

        // Assert =============================================================
        assertTrue( checkIdentity(expected, actual, sizeof(Frame)) );
    }
}


/*!
    @brief 全てのスロットへの、モーションフレーム全体の設定テスト
*/
test(AllSlotAllFrame_SetFrame)
{
    #if TEST_HARD

    using namespace PLEN2::Motion;

    // Setup ==================================================================
    Frame expected, actual;

    for (uint8_t slot = SLOT_BEGIN; slot < SLOT_END; slot++)
    {
        for (uint8_t index = Frame::FRAME_BEGIN; index < Frame::FRAME_END; index++)
        {
            // Setup ==========================================================
            validRandomize(expected);

            // Run ============================================================
            Frame::set(slot, index, expected);
            Frame::get(slot, index, actual);

            // Assert =========================================================
            assertTrue( checkIdentity(expected, actual, sizeof(Frame)) );
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

    // Setup ==================================================================
    Header header;

    {
        // Run ================================================================
        bool expected = false;
        bool actual   = Header::set(SLOT_END, header);

        // Assert =============================================================
        assertEqual(expected, actual);
    }

    {
        // Setup ==============================================================
        header.frame_length = 0;

        // Run ================================================================
        bool expected = false;
        bool actual   = Header::set(0, header);

        // Assert =============================================================
        assertEqual(expected, actual);
    }

    {
        // Setup ==============================================================
        header.frame_length = Header::FRAMELENGTH_MAX + 1;

        // Run ================================================================
        bool expected = false;
        bool actual   = Header::set(0, header);

        // Assert =============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief ヘッダ読み込みにおける、異常系のテスト
*/
test(GetHeader_InvalidInputs)
{
    using namespace PLEN2::Motion;

    // Setup ==================================================================
    Header header;

    {
        // Run ================================================================
        bool expected = false;
        bool actual   = Header::get(SLOT_END, header);

        // Assert =============================================================
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
        // Run =================================================================
        bool expected = false;
        bool actual   = Frame::set(SLOT_END, 0, frame);

        // Assert ==============================================================
        assertEqual(expected, actual);
    }

    {
        // Run =================================================================
        bool expected = false;
        bool actual   = Frame::set(0, Frame::FRAME_END, frame);

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
        // Run =================================================================
        bool expected = false;
        bool actual   = Frame::get(SLOT_END, 0, frame);

        // Assert ==============================================================
        assertEqual(expected, actual);
    }

    {
        // Run =================================================================
        bool expected = false;
        bool actual   = Frame::get(0, Frame::FRAME_END, frame);

        // Assert ==============================================================
        assertEqual(expected, actual);
    }
}


/*!
    @brief アプリケーション・エントリポイント
*/
void setup()
{
    randomSeed( analogRead(PLEN2::Pin::RANDOM_DEVICE_IN) );

    PLEN2::System::begin();
    PLEN2::ExternalEEPROM::begin();

    while (!Serial); // for the Arduino Leonardo/Micro only.

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    Test::run();
}
