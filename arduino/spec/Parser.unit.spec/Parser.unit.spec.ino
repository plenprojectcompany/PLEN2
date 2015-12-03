#line 2 "Parser.unit.spec.ino"


#include <ArduinoUnit.h>
#include "Parser.h"


/*!
	@brief NilParserの動作テスト
*/
test(NilParser)
{
	// Setup ===================================================================
	Utility::NilParser np;

	// Run & Assert ============================================================
	{
		bool expected_result = true;
		bool actual_result   = np.parse("tEbkk6dau-");

		char expected_index  = 0;
		char actual_index    = np.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = np.parse("PbEaCJ2wUV-");

		char expected_index  = 0;
		char actual_index    = np.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = np.parse("FW_rs5-h4K");

		char expected_index  = 0;
		char actual_index    = np.index();

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
		bool expected_result = true;
		bool actual_result   = cgp.parse("a");

		char expected_index  = 0;
		char actual_index    = cgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = cgp.parse("b");

		char expected_index  = 1;
		char actual_index    = cgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = cgp.parse("ax");

		char expected_index  = 0;
		char actual_index    = cgp.index();

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
		bool expected_result = false;
		bool actual_result   = cgp.parse("A");

		char expected_index  = -1;
		char actual_index    = cgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = cgp.parse("c");

		char expected_index  = -1;
		char actual_index    = cgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = cgp.parse("");

		char expected_index  = -1;
		char actual_index    = cgp.index();

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
	const unsigned char ACCEPT_STRS_LENGTH = sizeof(ACCEPT_STRS) / sizeof(ACCEPT_STRS[0]);

	Utility::StringGroupParser sgp(ACCEPT_STRS, ACCEPT_STRS_LENGTH);

	// Run & Assert ============================================================
	{
		bool expected_result = true;
		bool actual_result   = sgp.parse("Aa");

		char expected_index  = 0;
		char actual_index    = sgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = sgp.parse("AA");

		char expected_index  = 0;
		char actual_index    = sgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = sgp.parse("BB");

		char expected_index  = 1;
		char actual_index    = sgp.index();

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
	const unsigned char ACCEPT_STRS_LENGTH = sizeof(ACCEPT_STRS) / sizeof(ACCEPT_STRS[0]);

	Utility::StringGroupParser sgp(ACCEPT_STRS, ACCEPT_STRS_LENGTH);

	// Run & Assert ============================================================
	{
		bool expected_result = false;
		bool actual_result   = sgp.parse("A");

		char expected_index  = -1;
		char actual_index    = sgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = sgp.parse("CC");

		char expected_index  = -1;
		char actual_index    = sgp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = sgp.parse("");

		char expected_index  = -1;
		char actual_index    = sgp.index();

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
		bool expected_result = true;
		bool actual_result   = hsp.parse("00");

		char expected_index  = 0;
		char actual_index    = hsp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = hsp.parse("AF");

		char expected_index  = 0;
		char actual_index    = hsp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = true;
		bool actual_result   = hsp.parse("Bb");

		char expected_index  = 0;
		char actual_index    = hsp.index();

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
		bool expected_result = false;
		bool actual_result   = hsp.parse("-1");

		char expected_index  = -1;
		char actual_index    = hsp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = hsp.parse("+A");

		char expected_index  = -1;
		char actual_index    = hsp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}

	{
		bool expected_result = false;
		bool actual_result   = hsp.parse("GZ");

		char expected_index  = -1;
		char actual_index    = hsp.index();

		assertEqual(expected_result, actual_result);
		assertEqual(expected_index,  actual_index );
	}
}


/*!
	@brief hexbytes2uintの動作テスト
*/
test(hexbytes2uint)
{
	// Setup ===================================================================
	false;

	// Run & Assert ============================================================
	{
		unsigned int expected = 1;
		unsigned int actual   = Utility::hexbytes2uint("1", 1);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 15;
		unsigned int actual   = Utility::hexbytes2uint("F", 1);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 17;
		unsigned int actual   = Utility::hexbytes2uint("11", 2);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 255;
		unsigned int actual   = Utility::hexbytes2uint("FF", 2);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 273;
		unsigned int actual   = Utility::hexbytes2uint("111", 3);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 4095;
		unsigned int actual   = Utility::hexbytes2uint("FFF", 3);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 4369;
		unsigned int actual   = Utility::hexbytes2uint("1111", 4);

		assertEqual(expected, actual);
	}

	{
		unsigned int expected = 65535;
		unsigned int actual   = Utility::hexbytes2uint("FFFF", 4);

		assertEqual(expected, actual);
	}
}


/*!
	@brief hexbytes2intの動作テスト
*/
test(hexbytes2int)
{
	// Setup ===================================================================
	false;

	// Run & Assert ============================================================
	{
		int expected = 1;
		int actual   = Utility::hexbytes2int("1", 1);

		assertEqual(expected, actual);
	}

	{
		int expected = -1;
		int actual   = Utility::hexbytes2int("F", 1);

		assertEqual(expected, actual);
	}

	{
		int expected = 17;
		int actual   = Utility::hexbytes2int("11", 2);

		assertEqual(expected, actual);
	}

	{
		int expected = -1;
		int actual   = Utility::hexbytes2int("FF", 2);

		assertEqual(expected, actual);
	}

	{
		int expected = 273;
		int actual   = Utility::hexbytes2int("111", 3);

		assertEqual(expected, actual);
	}

	{
		int expected = -1;
		int actual   = Utility::hexbytes2int("FFF", 3);

		assertEqual(expected, actual);
	}

	{
		int expected = 4369;
		int actual   = Utility::hexbytes2int("1111", 4);

		assertEqual(expected, actual);
	}

	{
		int expected = -1;
		int actual   = Utility::hexbytes2int("FFFF", 4);

		assertEqual(expected, actual);
	}
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.print(F("# Test : "));
	Serial.println(__FILE__);
}

void loop()
{
	Test::run();
}