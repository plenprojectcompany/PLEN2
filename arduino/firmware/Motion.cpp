/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>

#include "ExternalEEPROM.h"
#include "Motion.h"
#include "JointController.h"

#if DEBUG
    #include "System.h"
    #include "Profiler.h"
#endif


namespace
{
    using namespace PLEN2;
    using namespace PLEN2::Motion;


    template<const int N>
    struct IF
    {
        enum { VALUE = 1 };
    };

    template<>
    struct IF<0>
    {
        enum { VALUE = 0 };
    };

    template<typename T>
    struct SIZE_SUP
    {
        enum { VALUE = sizeof(T) % ExternalEEPROM::SLOT_SIZE };
    };

    template<typename T>
    struct SLOT_COUNT
    {
        enum
        {
            VALUE = sizeof(T) / ExternalEEPROM::SLOT_SIZE + IF<SIZE_SUP<T>::VALUE>::VALUE
        };
    };

    enum
    {
        SLOT_COUNT_HEADER = SLOT_COUNT<Header>::VALUE,
        SLOT_COUNT_FRAME  = SLOT_COUNT<Frame>::VALUE,
        SLOT_COUNT_MOTION = SLOT_COUNT_HEADER + SLOT_COUNT_FRAME * Header::FRAMELENGTH_MAX
    };
}


namespace PLEN2
{
namespace Motion
{

void Header::init(Header& header)
{
    #if DEBUG
        PROFILING("Header::init()");
    #endif


    header.slot              = 0;
    header.name[0]           = '\0';
    header.name[NAME_LENGTH] = '\0';
    header.frame_length      = FRAMELENGTH_MIN;
    header.use_extra         = 0;
    header.use_jump          = 0;
    header.use_loop          = 0;
    header.loop_begin        = 0;
    header.loop_end          = 0;
    header.loop_count        = 0;
    header.jump_slot         = 0;
}


bool Header::set(uint8_t slot, const Header& header)
{
    #if DEBUG
        PROFILING("Header::set()");
    #endif


    if (slot >= SLOT_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument : slot = "));
            System::debugSerial().println(static_cast<int>(slot));
        #endif

        return false;
    }

    if (   (header.frame_length < FRAMELENGTH_MIN)
        || (header.frame_length > FRAMELENGTH_MAX)
    )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad instance : header.frame_length = "));
            System::debugSerial().println(static_cast<int>(header.frame_length));
        #endif

        return false;
    }


    const uint8_t* filler = reinterpret_cast<const uint8_t*>(&header);

    for (uint16_t count = 0; count < SLOT_COUNT_HEADER; count++)
    {
        int8_t result = ExternalEEPROM::writeSlot(
            (
                static_cast<uint16_t>(slot) * SLOT_COUNT_MOTION + count
            ),
            (
                filler + count * ExternalEEPROM::SLOT_SIZE
            ),
            (
                (count == (SLOT_COUNT_HEADER - 1))?
                    (
                        (SIZE_SUP<Header>::VALUE)?
                            SIZE_SUP<Header>::VALUE : ExternalEEPROM::SLOT_SIZE
                    )
                    : ExternalEEPROM::SLOT_SIZE
            )
        );

        if (result != 0)
        {
            #if DEBUG
                System::debugSerial().print(F(">>> failed : result["));
                System::debugSerial().print(static_cast<int>(count));
                System::debugSerial().print(F("] = "));
                System::debugSerial().println(static_cast<int>(result));
            #endif

            return false;
        }
    }

    return true;
}


bool Header::get(uint8_t slot, Header& header)
{
    #if DEBUG
        PROFILING("Header::get()");
    #endif


    if (slot >= SLOT_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument : slot = "));
            System::debugSerial().println(static_cast<int>(slot));
        #endif

        return false;
    }


    uint8_t* filler = reinterpret_cast<uint8_t*>(&header);

    for (uint16_t count = 0; count < SLOT_COUNT_HEADER; count++)
    {
        int8_t result = ExternalEEPROM::readSlot(
            (
                static_cast<uint16_t>(slot) * SLOT_COUNT_MOTION + count
            ),
            (
                filler + count * ExternalEEPROM::SLOT_SIZE
            ),
            (
                (count == (SLOT_COUNT_HEADER - 1))?
                    (
                        (SIZE_SUP<Header>::VALUE)?
                            SIZE_SUP<Header>::VALUE : ExternalEEPROM::SLOT_SIZE
                    )
                    : ExternalEEPROM::SLOT_SIZE
            )
        );

        if (result == -1)
        {
            #if DEBUG
                System::debugSerial().print(F(">>> failed : result["));
                System::debugSerial().print(static_cast<int>(count));
                System::debugSerial().print(F("] = "));
                System::debugSerial().println(static_cast<int>(result));
            #endif

            return false;
        }
    }

    return true;
}


void Frame::init(Frame& frame)
{
    #if DEBUG
        PROFILING("Frame::init()");
    #endif


    frame.index              = 0;
    frame.transition_time_ms = 32;

    for (uint8_t index = 0; index < JointController::JOINTS_SUM; index++)
    {
        frame.joint_angle[index] = 0;
    }
}


bool Frame::set(uint8_t slot, uint8_t index, const Frame& frame)
{
    #if DEBUG
        PROFILING("Frame::set()");
    #endif


    if (slot >= SLOT_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument : slot = "));
            System::debugSerial().println(static_cast<int>(slot));
        #endif

        return false;
    }

    if (index >= FRAME_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument : index = "));
            System::debugSerial().println(static_cast<int>(index));
        #endif

        return false;
    }


    const uint8_t* filler = reinterpret_cast<const uint8_t*>(&frame);

    for (uint16_t count = 0; count < SLOT_COUNT_FRAME; count++)
    {
        int8_t result = ExternalEEPROM::writeSlot(
            (
                  static_cast<uint16_t>(slot) * SLOT_COUNT_MOTION
                + SLOT_COUNT_HEADER
                + static_cast<uint16_t>(index) * SLOT_COUNT_FRAME
                + count
            ),
            (
                filler + ExternalEEPROM::SLOT_SIZE * count
            ),
            (
                (count == (SLOT_COUNT_FRAME - 1))?
                    (
                        (SIZE_SUP<Frame>::VALUE)?
                            SIZE_SUP<Frame>::VALUE : ExternalEEPROM::SLOT_SIZE
                    )
                    : ExternalEEPROM::SLOT_SIZE
            )
        );

        if (result != 0)
        {
            #if DEBUG
                System::debugSerial().print(F(">>> failed : result["));
                System::debugSerial().print(static_cast<int>(count));
                System::debugSerial().print(F("] = "));
                System::debugSerial().println(static_cast<int>(result));
            #endif

            return false;
        }
    }

    return true;
}


bool Frame::get(uint8_t slot, uint8_t index, Frame& frame)
{
    #if DEBUG
        PROFILING("Frame::get()");
    #endif


    if (slot >= SLOT_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument : slot = "));
            System::debugSerial().println(static_cast<int>(slot));
        #endif

        return false;
    }

    if (index >= Frame::FRAME_END)
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad instance : frame.index = "));
            System::debugSerial().println(static_cast<int>(index));
        #endif

        return false;
    }


    uint8_t* filler = reinterpret_cast<uint8_t*>(&frame);

    for (uint16_t count = 0; count < SLOT_COUNT_FRAME; count++)
    {
        int8_t result = ExternalEEPROM::readSlot(
            (
                  static_cast<uint16_t>(slot) * SLOT_COUNT_MOTION
                + SLOT_COUNT_HEADER
                + static_cast<uint16_t>(index) * SLOT_COUNT_FRAME
                + count
            ),
            (
                filler + ExternalEEPROM::SLOT_SIZE * count
            ),
            (
                (count == (SLOT_COUNT_FRAME - 1))?
                    (
                        (SIZE_SUP<Frame>::VALUE)?
                            SIZE_SUP<Frame>::VALUE : ExternalEEPROM::SLOT_SIZE
                    )
                    : ExternalEEPROM::SLOT_SIZE
            )
        );

        if (result == -1)
        {
            #if DEBUG
                System::debugSerial().print(F(">>> failed : result["));
                System::debugSerial().print(static_cast<int>(count));
                System::debugSerial().print(F("] = "));
                System::debugSerial().println(static_cast<int>(result));
            #endif

            return false;
        }
    }

    return true;
}

} // end of namespace "Motion".
} // end of namespace "PLEN2".