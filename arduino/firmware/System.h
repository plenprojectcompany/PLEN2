/*!
    @file      System.h
    @brief     Management class of basis about AVR MCU.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_SYSTEM_H
#define PLEN2_SYSTEM_H


#include "BuildConfig.h"

/*!
    @brief Definition of the firmware vesion
*/
#define FIRMWARE_VERSION "1.4.1"

class Stream;
class __FlashStringHelper;


namespace PLEN2
{
    class System;
}

/*!
    @brief Management class of basis about AVR MCU
*/
class PLEN2::System
{
private:
    /*!
        @brief Device name
    */
    inline static const __FlashStringHelper* DEVICE()
    {
        #if TARGET_PLEN14
            return F("PLEN.D");
        #endif

        #if TARGET_PLEN20
            return F(
                "PLEN2"

                #if TARGET_DEVELOPER_EDITION
                    "-Dev."
                #endif
            );
        #endif
    }

    /*!
        @brief Code name of the firmware
    */
    inline static const __FlashStringHelper* CODENAME()
    {
        return F(
            "Cytisus"

            #if TARGET_MIRROR_EDITION
                ": Mirror Function Supported"
            #endif
        );
    }

    //! @brief Version number of the firmware
    inline static const __FlashStringHelper* VERSION() { return F(FIRMWARE_VERSION); }

    //! @brief Communication speed of USB serial
    enum { USBSERIAL_BAUDRATE = 2000000L };

    //! @brief Communication speed of BLE serial
    enum
    {
        #if TARGET_DEVELOPER_EDITION
            BLESERIAL_BAUDRATE = 115200L
        #else
            BLESERIAL_BAUDRATE = 2000000L
        #endif
    };

public:
    //! @brief Size of internal eeprom
    enum { INTERNAL_EEPROMSIZE = 1024 };

    /*!
        @brief Constructor
    */
    System() { begin(); }

    /*!
        @brief Static constructor
    */
    static void begin();

    /*!
        @brief Get USB-serial instance

        @return Reference of USB-serial instance
    */
    static Stream& USBSerial();

    /*!
        @brief Get BLE-serial instance

        @return Reference of BLE-serial instance
    */
    static Stream& BLESerial();

    /*!
        @brief Get input-serial instance

        @return Reference of input-serial instance
    */
    static Stream& inputSerial();

    /*!
        @brief Get output-serial instance

        @return Reference of output-serial instance
    */
    static Stream& outputSerial();

    /*!
        @brief Get debug-serial instance

        @return Reference of debug-serial instance
    */
    static Stream& debugSerial();

    /*!
        @brief Dump information of the system

        Outputs result in JSON format as below.
        @code
        {
            "device": <string>,
            "codename": <string>,
            "version": <string>
        }
        @endcode
    */
    static void dump();

    /*!
        @brief Show welcome message.
    */
    static void welcome();
};

#endif // PLEN2_SYSTEM_H
