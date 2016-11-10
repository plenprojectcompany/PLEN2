/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>

#include "Parser.h"
#include "Protocol.h"

#if DEBUG
    #include "System.h"
    #include "Profiler.h"
#endif


namespace
{
    namespace Shared
    {
        const char* CONTROLLER_SYMBOL[] = {
            "AD", // APPLY DIFF
            "AN", // APPLY NATIVE
            "HP", // HOME POSITION
            "MP", // Alias of PLAY MOTION, @attention It will obsolescent in firmware version 2.x.
            "MS", // Alias of STOP MOTION, @attention It will obsolescent in firmware version 2.x.
            "PM", // PLAY MOTION
            "SM"  // STOP MOTION
        };
        const uint8_t CONTROLLER_ARGS_STORE_LENGTH[] = {
            5,    // APPLY DIFF
            5,    // APPLY NATIVE
            0,    // HOME POSITION
            2,    // PLAY MOTION, @attention It will obsolescent in firmware version 2.x.
            0,    // STOP MOTION, @attention It will obsolescent in firmware version 2.x.
            2,    // PLAY MOTION
            0     // STOP MOTION
        };

        enum { CONTROLLER_SYMBOL_LENGTH = sizeof(CONTROLLER_SYMBOL) / sizeof(CONTROLLER_SYMBOL[0]) };
        Utility::StringGroupParser controller_parser(CONTROLLER_SYMBOL, CONTROLLER_SYMBOL_LENGTH);


        const char* INTERPRETER_SYMBOL[] = {
            "PO", // POP CODE
            "PU", // PUSH CODE
            "RI"  // RESET INTERPRETER
        };
        const uint8_t INTERPRETER_ARGS_STORE_LENGTH[] = {
            0,    // POP CODE
            4,    // PUSH CODE
            0     // RESET INTERPRETER
        };

        enum { INTERPRETER_SYMBOL_LENGTH = sizeof(INTERPRETER_SYMBOL) / sizeof(INTERPRETER_SYMBOL[0]) };
        Utility::StringGroupParser interpreter_parser(INTERPRETER_SYMBOL, INTERPRETER_SYMBOL_LENGTH);


        const char* SETTER_SYMBOL[] = {
            "HO", // HOME
            "JS", // JOINT SETTINGS
            "MA", // MAX
            "MF", // MOTION FRAME
            "MH", // MOTION HEADER
            "MI"  // MIN
        };
        const uint8_t SETTER_ARGS_STORE_LENGTH[] = {
            5,    // HOME
            0,    // RESET JOINT SETTINGS
            5,    // MAX
            104,  // MOTION FRAME
            35,   // MOTION HEADER
            5     // MIN
        };

        enum { SETTER_SYMBOL_LENGTH = sizeof(SETTER_SYMBOL) / sizeof(SETTER_SYMBOL[0]) };
        Utility::StringGroupParser setter_parser(SETTER_SYMBOL, SETTER_SYMBOL_LENGTH);


        const char* GETTER_SYMBOL[] = {
            "JS", // JOINT SETTINGS
            "MO", // MOTION
            "VI"  // VERSION INFORMATION
        };
        const uint8_t GETTER_ARGS_STORE_LENGTH[] = {
            0,    // JOINT SETTINGS
            2,    // MOTION
            0     // VERSION INFORMATION
        };

        enum { GETTER_SYMBOL_LENGTH = sizeof(GETTER_SYMBOL) / sizeof(GETTER_SYMBOL[0]) };
        Utility::StringGroupParser getter_parser(GETTER_SYMBOL, GETTER_SYMBOL_LENGTH);


        /*!
            @note
            $ : Controller  [Priority 0]
            # : Interpreter [Priority 1]
            > : Setter      [Priority 2]
            < : Getter      [Priority 3]
        */
        Utility::CharGroupParser header_parser("$#><");

        Utility::AbstractParser* command_parser[] = {
            &controller_parser,
            &interpreter_parser,
            &setter_parser,
            &getter_parser
        };

        const unsigned char* ARGS_STORE_LENGTH[] = {
            CONTROLLER_ARGS_STORE_LENGTH,
            INTERPRETER_ARGS_STORE_LENGTH,
            SETTER_ARGS_STORE_LENGTH,
            GETTER_ARGS_STORE_LENGTH
        };


        /*!
            @note
            Delegate m_store_length to trim length.
        */
        Utility::HexStringParser args_parser;


        /*!
            @note
            The instance is used temporary for a protocol that accepts any string.
        */
        Utility::NilParser nil_parser;
    }
}


void PLEN2::Protocol::m_abort()
{
    #if DEBUG
        PROFILING("Protocol::m_abort()");
    #endif


    m_store_length    = 1;
    m_state           = READY;
    m_installing      = false;
    m_buffer.position = 0;
}


PLEN2::Protocol::Protocol()
    : m_store_length(1)
    , m_state(READY)
    , m_installing(false)
{
    m_parser[HEADER_INCOMING]    = &Shared::header_parser;
    m_parser[COMMAND_INCOMING]   = Shared::command_parser[0];
    m_parser[ARGUMENTS_INCOMING] = &Shared::args_parser;
}


void PLEN2::Protocol::readByte(char byte)
{
    #if DEBUG
        PROFILING("Protocol::readByte()");
    #endif


    m_buffer.data[m_buffer.position] = byte;
    (++m_buffer.position) &= (Buffer::LENGTH - 1);

    m_buffer.data[m_buffer.position] = '\0';
}


bool PLEN2::Protocol::accept()
{
    #if DEBUG
        PROFILING("Protocol::accept()");
    #endif


    if (m_buffer.position < m_store_length)
    {
        return false;
    }

    if (m_parser[m_state]->parse(m_buffer.data) == false)
    {
        m_abort();

        return false;
    }

    return true;
}


void PLEN2::Protocol::transitState()
{
    #if DEBUG
        PROFILING("Protocol::transitState()");
    #endif


    beforeHook();

    switch (m_state)
    {
        case HEADER_INCOMING:
        {
            m_state = COMMAND_INCOMING;
            m_parser[COMMAND_INCOMING] = Shared::command_parser[m_parser[HEADER_INCOMING]->index()];
            m_store_length = 2;

            break;
        }

        case COMMAND_INCOMING:
        {
            m_state = ARGUMENTS_INCOMING;

            // Partial specialization for a command
            if (m_parser[HEADER_INCOMING]->index() == 2 /* := Setter */)
            {
                // If accepted SET MOTION HEADER command, change to no-validation mode.
                if (m_parser[COMMAND_INCOMING]->index() == 4 /* := MOTION HEADER */)
                {
                    m_parser[ARGUMENTS_INCOMING] = &Shared::nil_parser;
                }
            }

            uint8_t header_id = m_parser[HEADER_INCOMING ]->index();
            uint8_t cmd_id    = m_parser[COMMAND_INCOMING]->index();

            m_store_length = Shared::ARGS_STORE_LENGTH[header_id][cmd_id];

            // If satisfy the following condition, transit READY state because the command has no arguments.
            if (m_store_length == 0)
            {
                m_state = READY;
                m_store_length = 1;
            }

            break;
        }

        case ARGUMENTS_INCOMING:
        {
            m_state = READY;
            m_parser[ARGUMENTS_INCOMING] = &Shared::args_parser;
            m_store_length = 1;

            break;
        }

        default:
        {
            #if DEBUG
                System::debugSerial().println(F(">>> error : Undefined state was inputed."));
            #endif

            m_abort();
        }
    }

    m_buffer.position = 0;

    afterHook();
}


void PLEN2::Protocol::beforeHook()
{
    #if DEBUG
        PROFILING("Protocol::beforeHook()");
    #endif
}


void PLEN2::Protocol::afterHook()
{
    #if DEBUG
        PROFILING("Protocol::afterHook()");
    #endif
}
