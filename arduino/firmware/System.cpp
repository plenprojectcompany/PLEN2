/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>

#include "Pin.h"
#include "System.h"

#if DEBUG
    #include "Profiler.h"
#endif


#define PLEN2_SYSTEM_USBSERIAL Serial
#define PLEN2_SYSTEM_BLESERIAL Serial1


void PLEN2::System::begin()
{
    PLEN2_SYSTEM_BLESERIAL.begin(BLESERIAL_BAUDRATE);
    PLEN2_SYSTEM_USBSERIAL.begin(USBSERIAL_BAUDRATE);

    pinMode(Pin::RS485_TXD, OUTPUT);
    pinMode(Pin::LED_OUT,   OUTPUT);

    digitalWrite(Pin::RS485_TXD, LOW);
    digitalWrite(Pin::LED_OUT,   LOW);

    randomSeed( analogRead(Pin::RANDOM_DEVICE_IN) );
}


Stream& PLEN2::System::USBSerial()
{
    return PLEN2_SYSTEM_USBSERIAL;
}


Stream& PLEN2::System::BLESerial()
{
    return PLEN2_SYSTEM_BLESERIAL;
}


Stream& PLEN2::System::inputSerial()
{
    return PLEN2_SYSTEM_BLESERIAL;
}


Stream& PLEN2::System::outputSerial()
{
    return PLEN2_SYSTEM_USBSERIAL;
}

Stream& PLEN2::System::debugSerial()
{
    return PLEN2_SYSTEM_USBSERIAL;
}


void PLEN2::System::dump()
{
    #if DEBUG
        PROFILING("System::dump()");
    #endif


    outputSerial().println(F("{"));

    outputSerial().print(F("\t\"device\": \""));
    outputSerial().print(DEVICE());
    outputSerial().println(F("\","));

    outputSerial().print(F("\t\"codename\": \""));
    outputSerial().print(CODENAME());
    outputSerial().println(F("\","));

    outputSerial().print(F("\t\"version\": \""));
    outputSerial().print(VERSION());
    outputSerial().println(F("\""));

    outputSerial().println(F("}"));
}


void PLEN2::System::welcome()
{
    #if DEBUG
        PROFILING("System::welcome()");
    #endif


    outputSerial().println(
        F("Hello, I am PLEN! My system is up and running ver." FIRMWARE_VERSION ", Let me walk :)")
    );
}