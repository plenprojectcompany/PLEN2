#line 2 "Soul.operation.spec.ino"


#include <EEPROM.h>
#include <Wire.h>

#include "Pin.h"
#include "System.h"
#include "ExternalEEPROM.h"
#include "Parser.h"
#include "Protocol.h"
#include "AccelerationGyroSensor.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Soul.h"


namespace
{
    PLEN2::AccelerationGyroSensor sensor;
    PLEN2::JointController        joint_ctrl;
    PLEN2::MotionController       motion_ctrl(joint_ctrl);
    PLEN2::Soul                   soul(sensor, motion_ctrl);

    class OperationTest : public PLEN2::Protocol
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

    public:
        virtual void afterHook()
        {
            if (m_state == READY)
            {
                uint8_t header_id = m_parser[HEADER_INCOMING ]->index();
                uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

                if ((header_id == 0 /* := Controller */) && (cmd_id == 5 /* := PLAY MOTION */))
                {
                    playMotion();
                }

                if ((header_id == 0 /* := Controller */) && (cmd_id == 6 /* := STOP MOTION */))
                {
                    stopMotion();
                }

                // soul.userActionInputed();
            }
        }
    };

    OperationTest test_core;
}


/*!
    @brief Application Entry Point
*/
void setup()
{
    PLEN2::System::begin();
    PLEN2::ExternalEEPROM::begin();

    randomSeed( analogRead(PLEN2::Pin::RANDOM_DEVICE_IN) );

    joint_ctrl.loadSettings();

    delay(3000);


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

    soul.log();
    soul.action();
}
