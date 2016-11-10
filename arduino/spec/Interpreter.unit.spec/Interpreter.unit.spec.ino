#line 2 "Interpreter.unit.spec.ino"


#include <Wire.h>
#include <EEPROM.h>
#include <ArduinoUnit.h>

#include "System.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Interpreter.h"


namespace
{
    PLEN2::JointController  joint_ctrl;
    PLEN2::MotionController motion_ctrl(joint_ctrl);
    PLEN2::Interpreter      interpreter(motion_ctrl);
}


/*!
    @brief push操作とpop操作の等試行確認テスト
*/
test(PushPop_Equals)
{
    // Setup ===================================================================
    uint8_t push_count = 0;
    uint8_t pop_count  = 0;

    PLEN2::Interpreter::Code code;
    interpreter.reset();

    // Run =====================================================================
    while (interpreter.pushCode(code))
    {
        push_count++;
    }

    while (interpreter.popCode())
    {
        pop_count++;
    }

    // Assert ==================================================================
    assertEqual(push_count, pop_count);
}


/*!
    @brief push操作の限界試行テスト
*/
test(PushCode_Overflow)
{
    // Setup ===================================================================
    PLEN2::Interpreter::Code code;
    interpreter.reset();

    // Run =====================================================================
    bool expected = false;
    bool actual   = true;

    for (uint8_t i = 0; i < PLEN2::Interpreter::QUEUE_SIZE; i++)
    {
        actual = interpreter.pushCode(code);
    }

    // Assert ==================================================================
    assertEqual(expected, actual);
}


/*!
    @brief 空のインタプリタに対するpop操作テスト
*/
test(PopCode_Empty)
{
    // Setup ===================================================================
    interpreter.reset();

    // Run =====================================================================
    bool expected = false;
    bool actual   = interpreter.popCode();

    // Assert ==================================================================
    assertEqual(expected, actual);
}


/*!
    @brief push操作によってready状態に移行するかのテスト
*/
test(Ready)
{
    // Setup ===================================================================
    PLEN2::Interpreter::Code code;
    interpreter.reset();
    interpreter.pushCode(code);

    // Run =====================================================================
    bool expected = true;
    bool actual   = interpreter.ready();

    // Assert ==================================================================
    assertEqual(expected, actual);
}


/*!
    @brief アプリケーション・エントリポイント
*/
void setup()
{
    PLEN2::System::begin();

    while (!Serial); // for the Arduino Leonardo/Micro only.

    PLEN2::System::outputSerial().print(F("# Test : "));
    PLEN2::System::outputSerial().println(__FILE__);
}

void loop()
{
    Test::run();
}
