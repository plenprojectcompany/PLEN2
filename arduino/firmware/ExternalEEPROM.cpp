/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>
#include <Wire.h>

#include "ExternalEEPROM.h"

#if DEBUG
    #include "System.h"
    #include "Profiler.h"
#endif


void PLEN2::ExternalEEPROM::begin()
{
    Wire.begin();
    Wire.setClock(CLOCK);
}


int8_t PLEN2::ExternalEEPROM::readSlot(uint16_t slot, uint8_t data[], uint8_t read_size)
{
    #if DEBUG
        PROFILING("ExternalEEPROM::readSlot()");
    #endif


    if (   (slot >= SLOT_END)
        || (read_size > SLOT_SIZE)
    )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument! : slot = "));
            System::debugSerial().print(slot);
            System::debugSerial().print(F(", or read_size = "));
            System::debugSerial().println(read_size);
        #endif

        return -1;
    }


    uint16_t slave_address = ADDRESS;
    uint32_t data_address = static_cast<uint32_t>(slot) * CHUNK_SIZE;

    if (data_address >= (SIZE / 2))
    {
        slave_address |= _BV(SELECT_BIT); // Select the memory block B0 = 1.
        data_address  -= (SIZE / 2);
    }

    #if DEBUG
        System::debugSerial().print(F(">>> slave_address = "));
        System::debugSerial().println(slave_address, HEX);

        System::debugSerial().print(F(">>> data_address = "));
        System::debugSerial().println(data_address, HEX);
    #endif

    Wire.beginTransmission(static_cast<int>(slave_address));
    Wire.write(static_cast<uint8_t>(data_address >> 8));     // Sending targeted address's high byte.
    Wire.write(static_cast<uint8_t>(data_address & 0x00FF)); // Sending targeted address's low byte.

    if (Wire.endTransmission() == 0)
    {
        if (Wire.requestFrom(slave_address, read_size) == read_size)
        {
            for (uint8_t index = 0; index < read_size; index++)
            {
                data[index] = Wire.read();
            }

            return read_size;
        }
    }

    return -1;
}


int8_t PLEN2::ExternalEEPROM::writeSlot(uint16_t  slot, const uint8_t data[], uint8_t write_size)
{
    #if DEBUG
        PROFILING("ExternalEEPROM::writeSlot()");
    #endif


    if (   (slot >= SLOT_END)
        || (write_size > SLOT_SIZE)
    )
    {
        #if DEBUG
            System::debugSerial().print(F(">>> bad argument! : slot = "));
            System::debugSerial().print(slot);
            System::debugSerial().print(F(", or write_size = "));
            System::debugSerial().println(write_size);
        #endif

        return -1;
    }


    uint16_t slave_address = ADDRESS;
    uint32_t data_address = static_cast<uint32_t>(slot) * CHUNK_SIZE;

    if (data_address >= (SIZE / 2))
    {
        slave_address |= _BV(SELECT_BIT); // Select the memory block B0 = 1.
        data_address  -= (SIZE / 2);
    }

    #if DEBUG
        System::debugSerial().print(F(">>> slave_address : "));
        System::debugSerial().println(slave_address, HEX);

        System::debugSerial().print(F(">>> data_address : "));
        System::debugSerial().println(data_address, HEX);
    #endif

    Wire.beginTransmission(static_cast<int>(slave_address));
    Wire.write(static_cast<uint8_t>(data_address >> 8));     // Sending targeted address's high byte.
    Wire.write(static_cast<uint8_t>(data_address & 0x00ff)); // Sending targeted address's low byte.

    for (uint8_t index = 0; index < write_size; index++)
    {
        Wire.write(data[index]);
    }

    int8_t ret = Wire.endTransmission();

    delay(5); // @attention Wait for writing EEPROM.

    return ret;
}
