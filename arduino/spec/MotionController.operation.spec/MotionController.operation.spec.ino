#line 2 "MotionController.operation.spec.ino"


#include <Wire.h>
#include <EEPROM.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "Parser.h"
#include "Protocol.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"


namespace
{
    PLEN2::JointController  joint_ctrl;
    PLEN2::MotionController motion_ctrl(joint_ctrl);


    class OperationTest: public PLEN2::Protocol
    {
    private:
        void playMotion()
        {
            motion_ctrl.play(
                Utility::hexbytes2uint16<2>(m_buffer.data)
            );
        }

        void stopMotion()
        {
            motion_ctrl.willStop();
        }

        void getMotion()
        {
            motion_ctrl.dump(
                Utility::hexbytes2uint16<2>(m_buffer.data)
            );
        }

    public:
        virtual void afterHook()
        {
            if (m_state == READY)
            {
                uint8_t header_id = m_parser[HEADER_INCOMING]->index();
                uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

                if ((header_id == 0 /* := Controller */) && (cmd_id == 5 /* := PLAY MOTION */))
                {
                    playMotion();
                }

                if ((header_id == 0 /* := Controller */) && (cmd_id == 6 /* := STOP MOTION */))
                {
                    stopMotion();
                }

                if ((header_id == 3 /* := Getter     */) && (cmd_id == 1 /* := MOTION      */))
                {
                    getMotion();
                }
            }
        }
    };

    OperationTest test_core;
}


/*!
    @brief アプリケーション・エントリポイント
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