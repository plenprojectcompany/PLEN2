#line 2 "System.unit.spec.ino"


#include <ArduinoUnit.h>

#include "System.h"


/*!
    @brief テストケース選択用プリプロセスマクロ
*/
#define TEST_USER true //!< ユーザテストについても実行します。


/*!
    @brief システム構成のダンプテスト

    ユーザによる目視でのテストです。
*/
test(Dump)
{
    #if TEST_USER
        PLEN2::System::dump();

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
    PLEN2::System::begin();

    while (!Serial); // for the Arduino Leonardo/Micro only.

    Serial.print(F("# Test : "));
    Serial.println(__FILE__);
}

void loop()
{
    Test::run();
}
