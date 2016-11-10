/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

/*!
    @note
    If you want to apply natural moving on PLEN, set the macro to "true".
    (The process is using a lot of calculation space, so the firmware becomes hard to input user action.)
*/
#define ENSOUL_PLEN2 false


#include <string.h>

#include <EEPROM.h>
#include <Wire.h>

#include "ExternalEEPROM.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Interpreter.h"
#include "Pin.h"
#include "Parser.h"
#include "Protocol.h"
#include "System.h"

#if ENSOUL_PLEN2
    #include "AccelerationGyroSensor.h"
    #include "Soul.h"
#endif

#if DEBUG
    #include "Profiler.h"
#endif


namespace
{
    using namespace PLEN2;

    /*!
        Core instances
    */
    JointController  joint_ctrl;
    MotionController motion_ctrl(joint_ctrl);
    Interpreter      interpreter(motion_ctrl);

    #if ENSOUL_PLEN2
        AccelerationGyroSensor sensor;
        Soul                   soul(sensor, motion_ctrl);
    #endif

    /*!
        The application instance
    */
    class Application : public Protocol
    {
    private:
        static void (Application::*CONTROLLER_EVENT_HANDLER[])();
        static void (Application::*INTERPRETER_EVENT_HANDLER[])();
        static void (Application::*SETTER_EVENT_HANDLER[])();
        static void (Application::*GETTER_EVENT_HANDLER[])();

        static void (Application::**EVENT_HANDLER[])();

        Motion::Header    m_header_tmp;
        Motion::Frame     m_frame_tmp;
        Interpreter::Code m_code_tmp;

        void applyDiff()
        {
            struct args
            {
                static uint16_t joint_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static int16_t angle_diff(char data[])
                {
                    return Utility::hexbytes2int16<3>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::applyDiff()");

                System::debugSerial().print(F(">>> joint_id : "));
                System::debugSerial().println(args::joint_id(m_buffer.data));

                System::debugSerial().print(F(">>> angle_diff : "));
                System::debugSerial().println(args::angle_diff(m_buffer.data));
            #endif

            joint_ctrl.setAngleDiff(
                args::joint_id(m_buffer.data), args::angle_diff(m_buffer.data)
            );
        }

        void apply()
        {
            struct args
            {
                static uint16_t joint_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static int16_t angle(char data[])
                {
                    return Utility::hexbytes2int16<3>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::apply()");

                System::debugSerial().print(F(">>> joint_id : "));
                System::debugSerial().println(args::joint_id(m_buffer.data));

                System::debugSerial().print(F(">>> angle : "));
                System::debugSerial().println(args::angle(m_buffer.data));
            #endif

            joint_ctrl.setAngle(
                args::joint_id(m_buffer.data), args::angle(m_buffer.data)
            );
        }

        void homePosition()
        {
            #if DEBUG
                PROFILING("Application::homePosition()");
            #endif

            joint_ctrl.loadSettings();
        }

        void playMotion()
        {
            struct args
            {
                static uint16_t slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }
            };

            #if DEBUG
                PROFILING("Application::playMotion()");

                System::debugSerial().print(F(">>> slot : "));
                System::debugSerial().println(args::slot(m_buffer.data));
            #endif

            motion_ctrl.play(args::slot(m_buffer.data));
        }

        void stopMotion()
        {
            #if DEBUG
                PROFILING("Application::stopMotion()");
            #endif

            motion_ctrl.willStop();
        }

        void popCode()
        {
            #if DEBUG
                PROFILING("Application::popCode()");
            #endif

            interpreter.popCode();
        }

        void pushCode()
        {
            struct args
            {
                static uint16_t slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static uint16_t loop_count(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::pushCode()");

                System::debugSerial().print(F(">>> slot : "));
                System::debugSerial().println(args::slot(m_buffer.data));

                System::debugSerial().print(F(">>> loop_count : "));
                System::debugSerial().println(args::loop_count(m_buffer.data));
            #endif

            /*!
                @attention
                Currently, official controller applications send loop_count that is 'expected value + 1',
                thus the firmware must adjust recieved loop_count subtracting 1.
            */
            m_code_tmp.slot       = args::slot(m_buffer.data);
            m_code_tmp.loop_count = args::loop_count(m_buffer.data) - 1; // (*)

            interpreter.pushCode(m_code_tmp);
        }

        void resetInterpreter()
        {
            #if DEBUG
                PROFILING("Application::resetInterpreter()");
            #endif

            interpreter.reset();
        }

        void setHome()
        {
            struct args
            {
                static uint16_t joint_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static int16_t angle(char data[])
                {
                    return Utility::hexbytes2int16<3>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::setHome()");

                System::debugSerial().print(F(">>> joint_id : "));
                System::debugSerial().println(args::joint_id(m_buffer.data));

                System::debugSerial().print(F(">>> angle : "));
                System::debugSerial().println(args::angle(m_buffer.data));
            #endif

            joint_ctrl.setHomeAngle(
                args::joint_id(m_buffer.data), args::angle(m_buffer.data)
            );
        }

        void setJointSettings()
        {
            #if DEBUG
                PROFILING("Application::setJointSettings()");
            #endif

            joint_ctrl.resetSettings();
        }

        void setMax()
        {
            struct args
            {
                static uint16_t joint_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static int16_t angle(char data[])
                {
                    return Utility::hexbytes2int16<3>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::setMax()");

                System::debugSerial().print(F(">>> joint_id : "));
                System::debugSerial().println(args::joint_id(m_buffer.data));

                System::debugSerial().print(F(">>> angle : "));
                System::debugSerial().println(args::angle(m_buffer.data));
            #endif

            joint_ctrl.setMaxAngle(
                args::joint_id(m_buffer.data),
                args::angle(m_buffer.data)
            );
        }

        void setMotionFrame()
        {
            struct args
            {
                static uint16_t slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static uint16_t frame_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 2);
                }

                static uint16_t transition_time_ms(char data[])
                {
                    return Utility::hexbytes2uint16<4>(data + 4);
                }

                static int16_t output(char data[], uint8_t device_id)
                {
                    return Utility::hexbytes2int16<4>(data + 8 + device_id * 4);
                }
            };

            #if DEBUG
                PROFILING("Application::setMotionFrame()");

                System::debugSerial().print(F(">>> slot : "));
                System::debugSerial().println(args::slot(m_buffer.data));

                System::debugSerial().print(F(">>> frame_id : "));
                System::debugSerial().println(args::frame_id(m_buffer.data));

                System::debugSerial().print(F(">>> transition_time_ms : "));
                System::debugSerial().println(args::transition_time_ms(m_buffer.data));

                for (uint8_t device_id = 0; device_id < JointController::JOINTS_SUM; device_id++)
                {
                    System::debugSerial().print(F(">>> output["));
                    System::debugSerial().print(device_id);
                    System::debugSerial().print(F("] : "));
                    System::debugSerial().println(args::output(m_buffer.data, device_id));
                }
            #endif

            m_frame_tmp.index              = args::frame_id(m_buffer.data);
            m_frame_tmp.transition_time_ms = args::transition_time_ms(m_buffer.data);

            for (uint8_t device_id = 0; device_id < JointController::JOINTS_SUM; device_id++)
            {
                m_frame_tmp.joint_angle[device_id] = args::output(m_buffer.data, device_id);
            }

            Motion::Frame::set(
                args::slot(m_buffer.data), args::frame_id(m_buffer.data), m_frame_tmp
            );
        }

        void setMotionHeader()
        {
            struct args
            {
                static uint16_t slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static const char* name(Motion::Header& header, char data[])
                {
                    strncpy(header.name, data + 2, 20);
                    header.name[20] = '\0';

                    return header.name;
                }

                static uint8_t use_loop(char data[])
                {
                    return ((data[22] != '0')? 1 : 0);
                }

                static uint8_t loop_begin(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 23);
                }

                static uint8_t loop_end(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 25);
                }

                static uint8_t loop_count(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 27);
                }

                static uint8_t use_jump(char data[])
                {
                    return ((data[29] != '0')? 1 : 0);
                }

                static uint8_t jump_slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 30);
                }

                static uint8_t use_extra(char data[])
                {
                    return ((data[32] != '0')? 1 : 0);
                }

                static uint8_t frame_length(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data + 33);
                }
            };

            #if DEBUG
                PROFILING("Application::setMotionHeader()");

                System::debugSerial().print(F(">>> slot : "));
                System::debugSerial().println(args::slot(m_buffer.data));

                System::debugSerial().print(F(">>> name : "));
                System::debugSerial().println(args::name(m_header_tmp, m_buffer.data));

                System::debugSerial().print(F(">>> use_loop : "));
                System::debugSerial().println(args::use_loop(m_buffer.data));

                System::debugSerial().print(F(">>> loop_begin : "));
                System::debugSerial().println(args::loop_begin(m_buffer.data));

                System::debugSerial().print(F(">>> loop_end : "));
                System::debugSerial().println(args::loop_end(m_buffer.data));

                System::debugSerial().print(F(">>> loop_count : "));
                System::debugSerial().println(args::loop_count(m_buffer.data));

                System::debugSerial().print(F(">>> use_jump : "));
                System::debugSerial().println(args::use_jump(m_buffer.data));

                System::debugSerial().print(F(">>> jump_slot : "));
                System::debugSerial().println(args::jump_slot(m_buffer.data));

                System::debugSerial().print(F(">>> frame_length : "));
                System::debugSerial().println(args::frame_length(m_buffer.data));
            #endif

            m_header_tmp.slot         = args::slot(m_buffer.data);
            /* m_header_tmp.name */     args::name(m_header_tmp, m_buffer.data);
            m_header_tmp.frame_length = args::frame_length(m_buffer.data);
            m_header_tmp.use_loop     = args::use_loop(m_buffer.data);
            m_header_tmp.loop_begin   = args::loop_begin(m_buffer.data);
            m_header_tmp.loop_end     = args::loop_end(m_buffer.data);
            m_header_tmp.loop_count   = args::loop_count(m_buffer.data);
            m_header_tmp.use_jump     = args::use_jump(m_buffer.data);
            m_header_tmp.jump_slot    = args::jump_slot(m_buffer.data);

            Motion::Header::set(args::slot(m_buffer.data), m_header_tmp);
        }

        void setMin()
        {
            struct args
            {
                static uint16_t joint_id(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }

                static int16_t angle(char data[])
                {
                    return Utility::hexbytes2int16<3>(data + 2);
                }
            };

            #if DEBUG
                PROFILING("Application::setMin()");

                System::debugSerial().print(F(">>> joint_id : "));
                System::debugSerial().println(args::joint_id(m_buffer.data));

                System::debugSerial().print(F(">>> angle : "));
                System::debugSerial().println(args::angle(m_buffer.data));
            #endif

            joint_ctrl.setMinAngle(
                args::joint_id(m_buffer.data), args::angle(m_buffer.data)
            );
        }

        void getJointSettings()
        {
            #if DEBUG
                PROFILING("Application::getJointSettings()");
            #endif

            joint_ctrl.dump();
        }

        void getMotion()
        {
            struct args
            {
                static uint16_t slot(char data[])
                {
                    return Utility::hexbytes2uint16<2>(data);
                }
            };

            #if DEBUG
                PROFILING("Application::getMotion()");

                System::debugSerial().print(F(">>> slot : "));
                System::debugSerial().println(args::slot(m_buffer.data));
            #endif

            motion_ctrl.dump(args::slot(m_buffer.data));
        }

        void getVersionInformation()
        {
            #if DEBUG
                PROFILING("Application::getVersionInformation()");
            #endif

            System::dump();
        }

    public:
        virtual void afterHook()
        {
            #if DEBUG
                PROFILING("Application::afterFook()");
            #endif

            if (m_state == HEADER_INCOMING)
            {
                uint8_t header_id = m_parser[HEADER_INCOMING ]->index();
                uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

                (this->*EVENT_HANDLER[header_id][cmd_id])();

                #if ENSOUL_PLEN2
                    soul.userActionInputed();
                #endif
            }
        }
    };

    void (Application::*Application::CONTROLLER_EVENT_HANDLER[])() = {
        &Application::applyDiff,
        &Application::apply,
        &Application::homePosition,
        &Application::playMotion,
        &Application::stopMotion,
        &Application::playMotion,
        &Application::stopMotion
    };

    void (Application::*Application::INTERPRETER_EVENT_HANDLER[])() = {
        &Application::popCode,
        &Application::pushCode,
        &Application::resetInterpreter
    };

    void (Application::*Application::SETTER_EVENT_HANDLER[])() = {
        &Application::setHome,
        &Application::setJointSettings,
        &Application::setMax,
        &Application::setMotionFrame,
        &Application::setMotionHeader,
        &Application::setMin
    };

    void (Application::*Application::GETTER_EVENT_HANDLER[])() = {
        &Application::getJointSettings,
        &Application::getMotion,
        &Application::getVersionInformation
    };

    void (Application::**Application::EVENT_HANDLER[])() = {
        Application::CONTROLLER_EVENT_HANDLER,
        Application::INTERPRETER_EVENT_HANDLER,
        Application::SETTER_EVENT_HANDLER,
        Application::GETTER_EVENT_HANDLER
    };

    Application app;
}


/*!
    @brief Setup

    Put your setup code here, to run once:

    @attention
    Digital pin's output is an indefinite if you don't give an initialize value.
    Please ensure that setup the pins which are configurable.

    @note
    The default baudrate of the firmware is defined at System.h.
    If you would like to change the value, please write down `Serial.begin()` in the function.
*/
void setup()
{
    PLEN2::System::begin();
    PLEN2::ExternalEEPROM::begin();

    joint_ctrl.loadSettings();

    #if ENSOUL_PLEN2
        /*!
            @attention
            The order of power supplied or firmware startup timing is base-board, head-board.
            If the sampling method calls from early timing, program freezes because synchronism of communication is missed.
            (Generally, it is going to success setup() inserts 3000[msec] delays.)
        */
        delay(3000);
    #endif

    #if DEBUG
        while (!Serial);

        PLEN2::System::welcome();
    #endif
}


/*!
    @brief Main polling loop

    Put your main code here, to run repeatedly:
*/
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
        app.readByte(PLEN2::System::USBSerial().read());

        if (app.accept())
        {
            app.transitState();

        }
    }

    if (PLEN2::System::BLESerial().available())
    {
        app.readByte(PLEN2::System::BLESerial().read());

        if (app.accept())
        {
            app.transitState();
        }
    }

    #if ENSOUL_PLEN2
        soul.log();
        soul.action();
    #endif
}
