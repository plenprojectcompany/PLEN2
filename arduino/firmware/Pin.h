/*!
    @file      Pin.h
    @brief     Management namespace of pin mapping.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_PIN_H
#define PLEN2_PIN_H


namespace PLEN2
{
    /*!
        @brief Management namespace of pin mapping

        Please give to the standard Arduino libraries methods the values defined in this namespace.
        The methods are evaluated at compile time, so there is no overhead at runtime.

        @note
        It helps your understanding to refer the PLEN2's circuit and schematic.
        -> https://github.com/plenprojectcompany/plen-ControlBoard

        @sa
        Arduino Micro's pin mapping -> http://arduino.cc/en/Hacking/PinMapping32u4
     */
    namespace Pin
    {
        //! @brief Output selection bit of multiplexer (0bit)
        enum { MULTIPLEXER_SELECT0 = 5 };

        //! @brief Output selection bit of multiplexer (1bit)
        enum { MULTIPLEXER_SELECT1 = 6 };

        //! @brief Output selection bit of multiplexer (2bit)
        enum { MULTIPLEXER_SELECT2 = 12 };

        //! @brief Output of PWM, for servo 00 to 07
        enum { PWM_OUT_00_07 = 11 };

        //! @brief Output of PWM, for servo 08 to 15
        enum { PWM_OUT_08_15 = 10 };

        //! @brief Output of PWM, for servo 16 to 23
        enum { PWM_OUT_16_23 = 9 };

        //! @brief Data flow selection bit of serial
        enum { RS485_TXD = 4 };

        //! @brief Output of LED
        enum { LED_OUT = 13 };

        //! @brief Input of random-device (Get an open circuit voltage.)
        enum { RANDOM_DEVICE_IN = 6 };
    }
}

#endif // PLEN2_PIN_H
