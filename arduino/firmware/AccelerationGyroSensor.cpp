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
#include "AccelerationGyroSensor.h"

#if DEBUG
    #include "Profiler.h"
#endif


namespace
{
    template<typename T>
    void endian_cast(T& value)
    {
        uint8_t  temp;
        uint8_t* filler = reinterpret_cast<uint8_t*>(&value);

        for (uint8_t index = 0; index < (sizeof(T) / 2); index++)
        {
            temp = filler[sizeof(T) - 1 - index];
            filler[sizeof(T) - 1 - index] = filler[index];
            filler[index] = temp;
        }
    }

    template<>
    void endian_cast(int16_t& value)
    {
        value = ((value & 0x00FF) << 8) | ((value >> 8) & 0x00FF);
    }
}


bool PLEN2::AccelerationGyroSensor::sampling()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::sampling()");
    #endif


    /*!
        @note
        Firstly, occupy the right of sending data (data flow is "base-board -> head-board") by substituting HIGH for Pin::RS485_TXD().
        If sending any data to head-board, the sensor responds values formatting 2byte, big-endian.

        Just after sending any data, must give up the right of sending data (data flow is "head-board -> base-board")
        by substituting LOW for Pin::RS485_TXD(), for receiving the values.
    */
    digitalWrite(Pin::RS485_TXD, HIGH);
    System::BLESerial().write('<');

    digitalWrite(Pin::RS485_TXD, LOW);

    uint8_t  read_count;
    uint8_t* filler = reinterpret_cast<uint8_t*>(m_values);

    read_count = System::BLESerial().readBytes(filler, SENSORS_SUM * sizeof(m_values[0]) + 1);

    if (read_count == (SENSORS_SUM * sizeof(m_values[0]) + 1))
    {
        for (uint8_t index = 0; index < SENSORS_SUM; index++)
        {
            endian_cast(m_values[index]);
        }

        return true;
    }

    return false;
}


const int16_t& PLEN2::AccelerationGyroSensor::getAccX()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getAccX()");
    #endif


    return m_values[ACC_X];
}


const int16_t& PLEN2::AccelerationGyroSensor::getAccY()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getAccY()");
    #endif


    return m_values[ACC_Y];
}


const int16_t& PLEN2::AccelerationGyroSensor::getAccZ()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getAccZ()");
    #endif


    return m_values[ACC_Z];
}


const int16_t& PLEN2::AccelerationGyroSensor::getGyroRoll()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getGyroRoll()");
    #endif


    return m_values[GYRO_ROLL];
}


const int16_t& PLEN2::AccelerationGyroSensor::getGyroPitch()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getGyroPitch()");
    #endif


    return m_values[GYRO_PITCH];
}


const int16_t& PLEN2::AccelerationGyroSensor::getGyroYaw()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::getGyroYaw()");
    #endif


    return m_values[GYRO_YAW];
}


void PLEN2::AccelerationGyroSensor::dump()
{
    #if DEBUG
        PROFILING("AccelerationGyroSensor::dump()");
    #endif


    System::outputSerial().println(F("{"));

    System::outputSerial().print(F("\t\"Acc X\": "));
    System::outputSerial().print(getAccX());
    System::outputSerial().println(F(","));

    System::outputSerial().print(F("\t\"Acc Y\": "));
    System::outputSerial().print(getAccY());
    System::outputSerial().println(F(","));

    System::outputSerial().print(F("\t\"Acc Z\": "));
    System::outputSerial().print(getAccZ());
    System::outputSerial().println(F(","));

    System::outputSerial().print(F("\t\"Gyro Roll\": "));
    System::outputSerial().print(getGyroRoll());
    System::outputSerial().println(F(","));

    System::outputSerial().print(F("\t\"Gyro Pitch\": "));
    System::outputSerial().print(getGyroPitch());
    System::outputSerial().println(F(","));

    System::outputSerial().print(F("\t\"Gyro Yaw\": "));
    System::outputSerial().println(getGyroYaw());

    System::outputSerial().println(F("}"));
}
