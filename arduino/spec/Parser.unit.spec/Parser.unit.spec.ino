#line 2 "Parser.unit.spec.ino"


#include <ArduinoUnit.h>
#include "Parser.h"


/*!
    @brief テストケース選択用プリプロセスマクロ
*/
#define TEST_COMPILE true //!< コンパイルタイムテストについても実行します。


/*!
    @brief NilParserの動作テスト
*/
test(NilParser)
{
    // Setup ===================================================================
    Utility::NilParser np;

    // Run & Assert ============================================================
    {
        bool   expected_result = true;
        bool   actual_result   = np.parse("tEbkk6dau-");

        int8_t expected_index  = 0;
        int8_t actual_index    = np.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = np.parse("PbEaCJ2wUV-");

        int8_t expected_index  = 0;
        int8_t actual_index    = np.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = np.parse("FW_rs5-h4K");

        int8_t expected_index  = 0;
        int8_t actual_index    = np.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief CharGroupParserの正常系に関する動作テスト
*/
test(CharGroupParser_ValidInputs)
{
    // Setup ===================================================================
    Utility::CharGroupParser cgp("ab");

    // Run & Assert ============================================================
    {
        bool   expected_result = true;
        bool   actual_result   = cgp.parse("a");

        int8_t expected_index  = 0;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = cgp.parse("b");

        int8_t expected_index  = 1;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = cgp.parse("ax");

        int8_t expected_index  = 0;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief CharGroupParserの異常系に関する動作テスト
*/
test(CharGroupParser_InvalidInputs)
{
    // Setup ===================================================================
    Utility::CharGroupParser cgp("ab");

    // Run & Assert ============================================================
    {
        bool   expected_result = false;
        bool   actual_result   = cgp.parse("A");

        int8_t expected_index  = -1;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = cgp.parse("c");

        int8_t expected_index  = -1;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = cgp.parse("");

        int8_t expected_index  = -1;
        int8_t actual_index    = cgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief StringGroupParserの正常系に関する動作テスト
*/
test(StringGroupParser_ValidInputs)
{
    // Setup ===================================================================
    const char* ACCEPT_STRS[] = {
        "AA",
        "BB"
    };
    enum { ACCEPT_STRS_LENGTH = sizeof(ACCEPT_STRS) / sizeof(ACCEPT_STRS[0]) };

    Utility::StringGroupParser sgp(ACCEPT_STRS, ACCEPT_STRS_LENGTH);

    // Run & Assert ============================================================
    {
        bool   expected_result = true;
        bool   actual_result   = sgp.parse("Aa");

        int8_t expected_index  = 0;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = sgp.parse("AA");

        int8_t expected_index  = 0;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = sgp.parse("BB");

        int8_t expected_index  = 1;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief StringGroupParserの異常系に関する動作テスト
*/
test(StringGroupParser_InvalidInputs)
{
    // Setup ===================================================================
    const char* ACCEPT_STRS[] = {
        "AA",
        "BB"
    };
    enum { ACCEPT_STRS_LENGTH = sizeof(ACCEPT_STRS) / sizeof(ACCEPT_STRS[0]) };

    Utility::StringGroupParser sgp(ACCEPT_STRS, ACCEPT_STRS_LENGTH);

    // Run & Assert ============================================================
    {
        bool   expected_result = false;
        bool   actual_result   = sgp.parse("A");

        int8_t expected_index  = -1;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = sgp.parse("CC");

        int8_t expected_index  = -1;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = sgp.parse("");

        int8_t expected_index  = -1;
        int8_t actual_index    = sgp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief HexStringParserの正常系に関する動作テスト
*/
test(HexStringParser_ValidInputs)
{
    // Setup ===================================================================
    Utility::HexStringParser hsp;

    // Run & Assert ============================================================
    {
        bool   expected_result = true;
        bool   actual_result   = hsp.parse("00");

        int8_t expected_index  = 0;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = hsp.parse("AF");

        int8_t expected_index  = 0;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = true;
        bool   actual_result   = hsp.parse("Bb");

        int8_t expected_index  = 0;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief HexStringParserの異常系に関する動作テスト
*/
test(HexStringParser_InvalidInputs)
{
    // Setup ===================================================================
    Utility::HexStringParser hsp;

    // Run & Assert ============================================================
    {
        bool   expected_result = false;
        bool   actual_result   = hsp.parse("-1");

        int8_t expected_index  = -1;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = hsp.parse("+A");

        int8_t expected_index  = -1;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }

    {
        bool   expected_result = false;
        bool   actual_result   = hsp.parse("GZ");

        int8_t expected_index  = -1;
        int8_t actual_index    = hsp.index();

        assertEqual(expected_result, actual_result);
        assertEqual(expected_index,  actual_index );
    }
}


/*!
    @brief hexbytes2uint16の動作テスト
*/
test(hexbytes2uint16)
{
    // Setup ===================================================================
    false;

    // Run & Assert ============================================================
    {
        uint16_t expected = 1;
        uint16_t actual   = Utility::hexbytes2uint16<1>("1");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 15;
        uint16_t actual   = Utility::hexbytes2uint16<1>("F");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 17;
        uint16_t actual   = Utility::hexbytes2uint16<2>("11");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 255;
        uint16_t actual   = Utility::hexbytes2uint16<2>("FF");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 273;
        uint16_t actual   = Utility::hexbytes2uint16<3>("111");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 4095;
        uint16_t actual   = Utility::hexbytes2uint16<3>("FFF");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 4369;
        uint16_t actual   = Utility::hexbytes2uint16<4>("1111");

        assertEqual(expected, actual);
    }

    {
        uint16_t expected = 65535;
        uint16_t actual   = Utility::hexbytes2uint16<4>("FFFF");

        assertEqual(expected, actual);
    }
}


/*!
    @brief hexbytes2int16の動作テスト
*/
test(hexbytes2int16)
{
    // Setup ===================================================================
    false;

    // Run & Assert ============================================================
    {
        int16_t expected = 1;
        int16_t actual   = Utility::hexbytes2int16<1>("1");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = -1;
        int16_t actual   = Utility::hexbytes2int16<1>("F");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = 17;
        int16_t actual   = Utility::hexbytes2int16<2>("11");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = -1;
        int16_t actual   = Utility::hexbytes2int16<2>("FF");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = 273;
        int16_t actual   = Utility::hexbytes2int16<3>("111");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = -1;
        int16_t actual   = Utility::hexbytes2int16<3>("FFF");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = 4369;
        int16_t actual   = Utility::hexbytes2int16<4>("1111");

        assertEqual(expected, actual);
    }

    {
        int16_t expected = -1;
        int16_t actual   = Utility::hexbytes2int16<4>("FFFF");

        assertEqual(expected, actual);
    }
}


/*!
    @brief 静的コンパイルエラーの検証
*/
#if TEST_COMPILE
void static_assertion_check()
{
    Utility::hexbytes2int16<5>("FFFFF");
    Utility::hexbytes2uint16<5>("FFFFF");
}
#endif


/*!
    @brief アプリケーション・エントリポイント
*/
void setup()
{
    Serial.begin(2000000);

    while (!Serial); // for the Arduino Leonardo/Micro only.

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    Test::run();
}