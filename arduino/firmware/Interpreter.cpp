/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>

#include "Interpreter.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"

#if DEBUG
    #include "System.h"
    #include "Profiler.h"
#endif


namespace
{
    inline uint8_t getIndex(uint8_t value)
    {
        return (value & (PLEN2::Interpreter::QUEUE_SIZE - 1));
    }
}


PLEN2::Interpreter::Interpreter(MotionController& motion_crtl)
    : m_queue_begin(0)
    , m_queue_end(0)
    , m_motion_ctrl_ptr(&motion_crtl)
{
    // no operations.
}


bool PLEN2::Interpreter::pushCode(const Code& code)
{
    #if DEBUG
        PROFILING("Interpreter::pushCode()");
    #endif


    if (getIndex(m_queue_end + 1) == m_queue_begin)
    {
        #if DEBUG
            System::debugSerial().println(F(">>> error : Queue overflow!"));
        #endif

        return false;
    }

    m_code_queue[m_queue_end] = code;
    m_queue_end = getIndex(m_queue_end + 1);

    return true;
}


bool PLEN2::Interpreter::popCode()
{
    #if DEBUG
        PROFILING("Interpreter::popCode()");
    #endif


    if (!ready())
    {
        #if DEBUG
            System::debugSerial().println(F(">>> error : This is not ready!"));
        #endif

        return false;
    }

    const Code& doing = m_code_queue[m_queue_begin];
    m_queue_begin = getIndex(m_queue_begin + 1);

    m_motion_ctrl_ptr->play(doing.slot);

    if (doing.loop_count != 0)
    {
        if (!m_motion_ctrl_ptr->m_header.use_loop)
        {
            m_motion_ctrl_ptr->m_header.use_loop = 1;

            m_motion_ctrl_ptr->m_header.loop_begin = 0;
            m_motion_ctrl_ptr->m_header.loop_end   = m_motion_ctrl_ptr->m_header.frame_length - 1;
        }
    }
    else
    {
        m_motion_ctrl_ptr->m_header.use_loop = 0;
    }

    m_motion_ctrl_ptr->m_header.use_jump = 0;
    m_motion_ctrl_ptr->m_header.loop_count = doing.loop_count;

    return true;
}


bool PLEN2::Interpreter::ready()
{
    #if DEBUG
        PROFILING("Interpreter::ready()");
    #endif


    return (m_queue_begin != m_queue_end);
}


void PLEN2::Interpreter::reset()
{
    #if DEBUG
        PROFILING("Interpreter::reset()");
    #endif


    m_queue_begin = 0;
    m_queue_end   = 0;
    m_motion_ctrl_ptr->stop();
}