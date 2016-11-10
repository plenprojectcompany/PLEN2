#line 2 "Interpreter.operation.spec.ino"


#include <Wire.h>
#include <EEPROM.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "Parser.h"
#include "Protocol.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Interpreter.h"


namespace
{
    PLEN2::JointController  joint_ctrl;
    PLEN2::MotionController motion_ctrl(joint_ctrl);
    PLEN2::Interpreter      interpreter(motion_ctrl);


    class OperationTest: public PLEN2::Protocol
    {
    private:
        void popCode()
        {
            interpreter.popCode();
        }

        void pushCode()
        {
            PLEN2::Interpreter::Code code;
            code.slot       = Utility::hexbytes2uint16<2>(m_buffer.data + 0);
            code.loop_count = Utility::hexbytes2uint16<2>(m_buffer.data + 2);

            interpreter.pushCode(code);
        }

        void resetInterpreter()
        {
            interpreter.reset();
        }

    public:
        virtual void afterHook()
        {
            if (m_state == READY)
            {
                uint8_t header_id = m_parser[HEADER_INCOMING ]->index();
                uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

                if ((header_id == 1 /* := Interpreter */) && (cmd_id == 0 /* := POP CODE          */))
                {
                    popCode();
                }

                if ((header_id == 1 /* := Interpreter */) && (cmd_id == 1 /* := PUSH CODE         */))
                {
                    pushCode();
                }

                if ((header_id == 1 /* := Interpreter */) && (cmd_id == 2 /* := RESET INTERPRETER */))
                {
                    resetInterpreter();
                }
            }
        }
    };

    OperationTest test_core;
}


/*!
    @brief Application entry point
*/
void setup()
{
    PLEN2::System::begin();
    PLEN2::ExternalEEPROM::begin();

    joint_ctrl.loadSettings();


    while (!Serial); // for the Arduino Leonardo/Micro only.

    PLEN2::System::outputSerial().print(F("# Test : "));
    PLEN2::System::outputSerial().println(__FILE__);
}

void loop()
{
    if (motion_ctrl.playing())
    {
        if (motion_ctrl.frameUpdatable())
        {
            motion_ctrl.updateFrame();
        }

        if (motion_ctrl.updatingFinished())
        {
            if (motion_ctrl.nextFrameLoadable())
            {
                motion_ctrl.loadNextFrame();
            }
            else
            {
                motion_ctrl.stop();

                if (interpreter.ready())
                {
                    interpreter.popCode();
                }
            }
        }
    }

    if (PLEN2::System::USBSerial().available())
    {
        test_core.readByte(PLEN2::System::USBSerial().read());

        if (test_core.accept())
        {
            test_core.transitState();
        }
    }

    if (PLEN2::System::BLESerial().available())
    {
        test_core.readByte(PLEN2::System::BLESerial().read());

        if (test_core.accept())
        {
            test_core.transitState();
        }
    }
}
