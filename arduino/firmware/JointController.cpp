/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG      false
#define DEBUG_HARD false

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <Arduino.h>
#include <EEPROM.h>

#include "BuildConfig.h"
#include "Pin.h"
#include "System.h"
#include "JointController.h"

#if DEBUG || DEBUG_HARD
    #include "Profiler.h"
#endif


#if TARGET_PLEN14
    #define CLOCK_WISE false
#endif

#if TARGET_PLEN20
    #define CLOCK_WISE true
#endif

#define PLEN2_JOINTCONTROLLER_PWM_OUT_00_07_REGISTER OCR1C
#define PLEN2_JOINTCONTROLLER_PWM_OUT_08_15_REGISTER OCR1B
#define PLEN2_JOINTCONTROLLER_PWM_OUT_16_23_REGISTER OCR1A


volatile bool PLEN2::JointController::m_1cycle_finished = false;
uint16_t PLEN2::JointController::m_pwms[PLEN2::JointController::JOINTS_SUM];


namespace
{
    namespace Shared
    {
        using namespace PLEN2;

        PROGMEM const int16_t m_SETTINGS_INITIAL[] =
        {
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  -40, // [00] Left : Shoulder Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  245, // [01] Left : Thigh Yaw
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  470, // [02] Left : Shoulder Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -100, // [03] Left : Elbow Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -205, // [04] Left : Thigh Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,   50, // [05] Left : Thigh Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  445, // [06] Left : Knee Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  245, // [07] Left : Foot Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  -75, // [08] Left : Foot Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,   15, // [12] Right : Shoulder Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  -70, // [13] Right : Thigh Yaw
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -390, // [14] Right : Shoulder Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  250, // [15] Right : Elbow Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,  195, // [16] Right : Thigh Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -105, // [17] Right : Thigh Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -510, // [18] Right : Knee Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, -305, // [19] Right : Foot Pitch
            JointController::ANGLE_MIN, JointController::ANGLE_MAX,   60, // [20] Right : Foot Roll
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL,
            JointController::ANGLE_MIN, JointController::ANGLE_MAX, JointController::ANGLE_NEUTRAL
        };

        const int16_t ERROR_LVALUE = -32768;
    }
}


PLEN2::JointController::JointController()
{
    pinMode(Pin::MULTIPLEXER_SELECT0, OUTPUT);
    pinMode(Pin::MULTIPLEXER_SELECT1, OUTPUT);
    pinMode(Pin::MULTIPLEXER_SELECT2, OUTPUT);
    pinMode(Pin::PWM_OUT_00_07,       OUTPUT);
    pinMode(Pin::PWM_OUT_08_15,       OUTPUT);
    pinMode(Pin::PWM_OUT_16_23,       OUTPUT);

    for (uint8_t joint_id = 0; joint_id < JOINTS_SUM; joint_id++)
    {
        m_SETTINGS[joint_id].MIN  = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 0);
        m_SETTINGS[joint_id].MAX  = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 1);
        m_SETTINGS[joint_id].HOME = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 2);

        setAngle(joint_id, m_SETTINGS[joint_id].HOME);
    }
}


void PLEN2::JointController::loadSettings()
{
    #if DEBUG
        PROFILING("JointController::loadSettings()");
    #endif


    uint8_t* filler = reinterpret_cast<uint8_t*>(m_SETTINGS);
    
    if (EEPROM[INIT_FLAG_ADDRESS] != INIT_FLAG_VALUE)
    {
        EEPROM[INIT_FLAG_ADDRESS] = INIT_FLAG_VALUE;
        eeprom_busy_wait();

        for (uint8_t index = 0; index < sizeof(m_SETTINGS); index++)
        {
            EEPROM[SETTINGS_HEAD_ADDRESS + index] = filler[index];
            eeprom_busy_wait();
        }
    }
    else
    {
        for (uint8_t index = 0; index < sizeof(m_SETTINGS); index++)
        {
            filler[index] = EEPROM[SETTINGS_HEAD_ADDRESS + index];
        }
    }

    for (uint8_t joint_id = 0; joint_id < JOINTS_SUM; joint_id++)
    {
        setAngle(joint_id, m_SETTINGS[joint_id].HOME);
    }

    /*
        @brief Configure timer 1

        @attention
        It might be easy to understand compare-matched output is LOW,
        but in that case, PWM is outputting at switching multiplexer's output,
        so the signal gets an impulse noise.
    */
    cli();

    TCCR1A =
        _BV(WGM11)  | _BV(WGM10)  | // Set mode to "10bit fast PWM".
        _BV(COM1A1) | _BV(COM1A0) | // Set OC1A to HIGH when compare matched.
        _BV(COM1B1) | _BV(COM1B0) | // Set OC1B to HIGH when compare matched.
        _BV(COM1C1) | _BV(COM1C0);  // Set OC1C to HIGH when compare matched.

    TCCR1B = 
        _BV(WGM12) |                // Set mode to "10bit fast PWM".
        _BV(CS11)  | _BV(CS10);     // Set prescaler to 64.

    TIFR1 = _BV(OCF1A) | _BV(OCF1B) | _BV(OCF1C) | _BV(TOV1); // Clearing interruption flag.

    sei();

    TIMSK1 = _BV(TOIE1); // Begin timer 1.
}


void PLEN2::JointController::resetSettings()
{
    #if DEBUG
        PROFILING("JointController::resetSettings()");
    #endif


    EEPROM[INIT_FLAG_ADDRESS] = INIT_FLAG_VALUE;
    eeprom_busy_wait();

    for (uint8_t index = 0; index < sizeof(Shared::m_SETTINGS_INITIAL); index++)
    {
        EEPROM[SETTINGS_HEAD_ADDRESS + index] = pgm_read_byte(
            reinterpret_cast<const uint8_t*>(Shared::m_SETTINGS_INITIAL) + index
        );
        eeprom_busy_wait();
    }

    for (uint8_t joint_id = 0; joint_id < JOINTS_SUM; joint_id++)
    {
        m_SETTINGS[joint_id].MIN  = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 0);
        m_SETTINGS[joint_id].MAX  = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 1);
        m_SETTINGS[joint_id].HOME = pgm_read_word(Shared::m_SETTINGS_INITIAL + joint_id * 3 + 2);

        setAngle(joint_id, m_SETTINGS[joint_id].HOME);
    }
}


const int16_t& PLEN2::JointController::getMinAngle(uint8_t joint_id)
{
    #if DEBUG
        PROFILING("JointController::getMinAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return Shared::ERROR_LVALUE;
    }

    return m_SETTINGS[joint_id].MIN;
}


const int16_t& PLEN2::JointController::getMaxAngle(uint8_t joint_id)
{
    #if DEBUG
        PROFILING("JointController::getMaxAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return Shared::ERROR_LVALUE;
    }

    return m_SETTINGS[joint_id].MAX;
}


const int16_t& PLEN2::JointController::getHomeAngle(uint8_t joint_id)
{
    #if DEBUG
        PROFILING("JointController::getHomeAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return Shared::ERROR_LVALUE;
    }

    return m_SETTINGS[joint_id].HOME;
}


bool PLEN2::JointController::setMinAngle(uint8_t joint_id, int16_t angle)
{
    #if DEBUG
        PROFILING("JointController::setMinAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return false;
    }

    if (   (angle >= m_SETTINGS[joint_id].MAX)
        || (angle <  ANGLE_MIN) )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : angle = "));
            System::debugSerial().println(angle);
        #endif

        return false;
    }


    m_SETTINGS[joint_id].MIN = angle;

    uint8_t* filler = reinterpret_cast<uint8_t*>(&(m_SETTINGS[joint_id].MIN));
    uint16_t address_offset = reinterpret_cast<uint16_t>(filler) - reinterpret_cast<uint16_t>(m_SETTINGS);

    #if DEBUG
        System::debugSerial().print(F(">>> address_offset : "));
        System::debugSerial().println(address_offset);
    #endif

    for (uint8_t index = 0; index < sizeof(m_SETTINGS[joint_id].MIN); index++)
    {
        EEPROM[SETTINGS_HEAD_ADDRESS + address_offset + index] = filler[index];
        eeprom_busy_wait();
    }

    return true;
}


bool PLEN2::JointController::setMaxAngle(uint8_t joint_id, int16_t angle)
{
    #if DEBUG
        PROFILING("JointController::setMaxAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return false;
    }

    if (   (angle <= m_SETTINGS[joint_id].MIN)
        || (angle >  ANGLE_MAX) )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : angle = "));
            System::debugSerial().println(angle);
        #endif

        return false;
    }


    m_SETTINGS[joint_id].MAX = angle;

    uint8_t* filler = reinterpret_cast<uint8_t*>(&(m_SETTINGS[joint_id].MAX));
    uint16_t address_offset = reinterpret_cast<uint16_t>(filler) - reinterpret_cast<uint16_t>(m_SETTINGS);

    #if DEBUG
        System::debugSerial().print(F(">>> address_offset : "));
        System::debugSerial().println(address_offset);
    #endif

    for (uint8_t index = 0; index < sizeof(m_SETTINGS[joint_id].MAX); index++)
    {
        EEPROM[SETTINGS_HEAD_ADDRESS + address_offset + index] = filler[index];
        eeprom_busy_wait();
    }

    return true;
}


bool PLEN2::JointController::setHomeAngle(uint8_t joint_id, int16_t angle)
{
    #if DEBUG
        PROFILING("JointController::setHomeAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return false;
    }

    if (   (angle < m_SETTINGS[joint_id].MIN)
        || (angle > m_SETTINGS[joint_id].MAX) )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argment! : angle = "));
            System::debugSerial().println(angle);
        #endif

        return false;
    }


    m_SETTINGS[joint_id].HOME = angle;

    uint8_t* filler = reinterpret_cast<uint8_t*>(&(m_SETTINGS[joint_id].HOME));
    uint16_t address_offset = reinterpret_cast<uint16_t>(filler) - reinterpret_cast<uint16_t>(m_SETTINGS);

    #if DEBUG
        System::debugSerial().print(F(">>> address_offset : "));
        System::debugSerial().println(address_offset);
    #endif

    for (uint8_t index = 0; index < sizeof(m_SETTINGS[joint_id].HOME); index++)
    {
        EEPROM[SETTINGS_HEAD_ADDRESS + address_offset + index] = filler[index];
        eeprom_busy_wait();
    }

    return true;
}


bool PLEN2::JointController::setAngle(uint8_t joint_id, int16_t angle)
{
    #if DEBUG_HARD
        PROFILING("JointController::setAngle()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG_HARD
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return false;
    }


    angle = constrain(angle, m_SETTINGS[joint_id].MIN, m_SETTINGS[joint_id].MAX);

    m_pwms[joint_id] = map(angle, ANGLE_MIN, ANGLE_MAX,
        #if CLOCK_WISE
            PWM_MIN, PWM_MAX
        #else
            PWM_MAX, PWM_MIN
        #endif
    );

    return true;
}


bool PLEN2::JointController::setAngleDiff(uint8_t joint_id, int16_t angle_diff)
{
    #if DEBUG_HARD
        PROFILING("JointController::setAngleDiff()");
    #endif


    if (joint_id >= JOINTS_SUM)
    {
        #if DEBUG_HARD
            System::debugSerial().print(F(">>> bad argment! : joint_id = "));
            System::debugSerial().println(static_cast<int>(joint_id));
        #endif

        return false;
    }


    int16_t angle = constrain(
        angle_diff + m_SETTINGS[joint_id].HOME,
        m_SETTINGS[joint_id].MIN, m_SETTINGS[joint_id].MAX
    );

    m_pwms[joint_id] = map(angle, ANGLE_MIN, ANGLE_MAX,
        #if CLOCK_WISE
            PWM_MIN, PWM_MAX
        #else
            PWM_MAX, PWM_MIN
        #endif
    );

    return true;
}


void PLEN2::JointController::dump()
{
    #if DEBUG
        PROFILING("JointController::dump()");
    #endif


    System::outputSerial().println(F("["));

    for (uint8_t joint_id = 0; joint_id < JOINTS_SUM; joint_id++)
    {
        System::outputSerial().println(F("\t{"));

        System::outputSerial().print(F("\t\t\"@device\": "));
        System::outputSerial().print(static_cast<int>(joint_id));
        System::outputSerial().println(F(","));

        System::outputSerial().print(F("\t\t\"max\": "));
        System::outputSerial().print(m_SETTINGS[joint_id].MAX);
        System::outputSerial().println(F(","));

        System::outputSerial().print(F("\t\t\"min\": "));
        System::outputSerial().print(m_SETTINGS[joint_id].MIN);
        System::outputSerial().println(F(","));

        System::outputSerial().print(F("\t\t\"home\": "));
        System::outputSerial().println(m_SETTINGS[joint_id].HOME);

        System::outputSerial().print(F("\t}"));

        if (joint_id != (JOINTS_SUM - 1))
        {
            System::outputSerial().println(F(","));
        }
        else
        {
            System::outputSerial().println();
        }
    }

    System::outputSerial().println(F("]"));
}


/*
    @brief Timer 1 overflow interruption vector

    The interruption vector runs at the moment TCNT1 overflowed.
    In the firmware, 16[MHz] clock source is prescaled by 64, and using 10bit mode,
    so interruption interval is (16,000,000 / (64 * 1,024))^-1 * 1,000 = 4.096[msec].

    The value is too smaller than servo's PWM acceptable interval,
    so the firmware can control 24 servos by outputting PWM once in 8 times
    and changing output line at each interruption timing.

    @attention
    Please add volatile prefix to all editable instance of the vector,
    for countermeasure of optimization.

    The MCU outputs PWM with double-buffering so joint selection should look ahead next joint.
    If fail to do the procedure, controlling plural servos is not to be you intended.
*/
ISR(TIMER1_OVF_vect)
{
    using namespace PLEN2;

    /*
        @attention
        **joint_select** looks ahead next joint considering double-buffering.
    */
    volatile static uint8_t output_select = 0;
    volatile static uint8_t joint_select  = 1;

    /*
        @attention
        Switch output lines at early timing in the interruption,
        because should be switched before PWM outputting for communication without a noise.

        @sa
        PLEN2::JointController::loadSettings()
    */
    digitalWrite(Pin::MULTIPLEXER_SELECT0, bitRead(output_select, 0));
    digitalWrite(Pin::MULTIPLEXER_SELECT1, bitRead(output_select, 1));
    digitalWrite(Pin::MULTIPLEXER_SELECT2, bitRead(output_select, 2));

    PLEN2_JOINTCONTROLLER_PWM_OUT_00_07_REGISTER = JointController::m_pwms[
        joint_select + 0 * JointController::Multiplexer::SELECTABLE_LINES
    ];

    PLEN2_JOINTCONTROLLER_PWM_OUT_08_15_REGISTER = JointController::m_pwms[
        joint_select + 1 * JointController::Multiplexer::SELECTABLE_LINES
    ];

    PLEN2_JOINTCONTROLLER_PWM_OUT_16_23_REGISTER = JointController::m_pwms[
        joint_select + 2 * JointController::Multiplexer::SELECTABLE_LINES
    ];

    (++output_select) &= (JointController::Multiplexer::SELECTABLE_LINES - 1);
    (++joint_select)  &= (JointController::Multiplexer::SELECTABLE_LINES - 1);

    (joint_select == 0)? (JointController::m_1cycle_finished = true) : false;
}
