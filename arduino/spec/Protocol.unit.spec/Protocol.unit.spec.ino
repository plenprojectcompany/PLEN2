#line 2 "Protocol.unit.spec.ino"


#include <Arduino.h>
#include <ArduinoUnit.h>

#include "System.h"
#include "Protocol.h"


namespace
{
    class TestProtocol: public PLEN2::Protocol
    {
    public:
        void abort()
        {
            m_abort();
        }

        void readString(const char* str)
        {
            while (*str != '\0')
            {
                readByte(*str++);
            }
        }
    };

    TestProtocol protocol;
}


/*!
    @brief 正常なヘッダ入力時の挙動テスト
*/
test(Header_ValidInputs)
{
    // Setup ===================================================================
    protocol.abort();

    // Run & Assert ============================================================
    {
        protocol.readByte('$');

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }

    {
        protocol.readByte('#');

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }

    {
        protocol.readByte('>');

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }

    {
        protocol.readByte('<');

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }
}


/*!
    @brief 異常なヘッダ入力時の挙動テスト
*/
test(Header_InvalidInputs)
{
    // Setup ===================================================================
    protocol.abort();

    // Run & Assert ============================================================
    {
        protocol.readByte('a');

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }

    {
        protocol.readByte('0');

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);

        protocol.abort();
    }
}


/*!
    @brief 正常なCONTROLLERコマンド入力時の挙動テスト
*/
test(Controller_ValidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('$');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("AD");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("AN");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("HP");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MP");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MS");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("PM");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("SM");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 異常なCONTROLLERコマンド入力時の挙動テスト
*/
test(Controller_InvalidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('$');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("A");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("M");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("ADXXXX");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 正常なINTERPRETERコマンド入力時の挙動テスト
*/
test(Interpreter_ValidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('#');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("PO");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("PU");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("RI");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 異常なINTERPRETERコマンド入力時の挙動テスト
*/
test(Interpreter_InvalidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('#');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("P");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("PUXXXX");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("0");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 正常なSETTERコマンド入力時の挙動テスト
*/
test(Setter_ValidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('>');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("HO");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("JS");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MA");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MF");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MH");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MI");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 異常なSETTERコマンド入力時の挙動テスト
*/
test(Setter_InvalidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('>');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("M");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("JSXXXX");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("0");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 正常なGETTERコマンド入力時の挙動テスト
*/
test(Getter_ValidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('<');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("JS");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MO");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("VI");

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 異常なGETTERコマンド入力時の挙動テスト
*/
test(Getter_InvalidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()()
        {
            protocol.abort();

            protocol.readByte('<');
            protocol.accept();
            protocol.transitState();
        }
    };

    Setup setup;

    // Run & Assert ============================================================
    {
        setup();

        protocol.readString("J");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("MOXXXX");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup();

        protocol.readString("0");

        bool expected = false;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


/*!
    @brief 正常なARGS入力時の挙動テスト
*/
test(Args_ValidInputs)
{
    // Setup ===================================================================
    struct Setup
    {
        void operator()(const char* input)
        {
            protocol.abort();

            protocol.readByte(input[0]);
            protocol.accept();
            protocol.transitState();

            protocol.readString(input + 1);
            protocol.accept();
            protocol.transitState();
        }
    };

    struct nInput
    {
        void operator()(const char input, int loop)
        {
            for (int index = 0; index < loop; index++)
            {
                protocol.readByte(input);
            }
        }
    };

    Setup  setup;
    nInput n_input;

    // Run & Assert ============================================================
    {
        setup("$AD");

        n_input('0', 5);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup("$AN");

        n_input('1', 5);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup("$MP");

        n_input('2', 2);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup("$PM");

        n_input('3', 2);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup("#PU");

        n_input('4', 4);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup(">HO");

        n_input('5', 5);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup(">MA");

        n_input('6', 5);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup(">MF");

        n_input('7', 104);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup(">MH");

        n_input('8', 35);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup(">MI");

        n_input('9', 5);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }

    {
        setup("<MO");

        n_input('A', 2);

        bool expected = true;
        bool actual   = protocol.accept();

        assertEqual(expected, actual);
    }
}


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
