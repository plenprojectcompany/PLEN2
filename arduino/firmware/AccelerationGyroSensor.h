/*!
    @file      AccelerationGyroSensor.h
    @brief     Management class of acceleration and gyro sensor.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_ACCELERATION_GYRO_SENSOR_H
#define PLEN2_ACCELERATION_GYRO_SENSOR_H


#include <stdint.h>

namespace PLEN2
{
    class AccelerationGyroSensor;
}

/*!
    @brief Management class of acceleration and gyro sensor
*/
class PLEN2::AccelerationGyroSensor
{
private:
    enum SENSOR_VALUE_MAP
    {
        ACC_X,
        ACC_Y,
        ACC_Z,
        GYRO_ROLL,
        GYRO_PITCH,
        GYRO_YAW,
        SENSORS_SUM
    };

    int16_t m_values[SENSORS_SUM + 1];

public:
    /*!
        @brief Do sampling sensor values

        Usage assumption is to call the method at stated periods from loop().

        @return Result

        @attention
        The method has a deadlock of interruption and communication wait
        because it communicate with the sensor through a bus,
        so using it in constructor and interruption vector is deprecated.
        <br><br>
        The order of power supplied or firmware startup timing is base-board, head-board.
        If the method calls from early timing, program freezes because synchronism of communication is missed.
        (Generally, after setup() inserts a 3000[msec] delays for success.)
    */
    bool sampling();

    /*!
        @brief Get acceleration on X axis

        @return Acceleration on X axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getAccX();

    /*!
        @brief Get acceleration on Y axis

        @return Acceleration on Y axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getAccY();

    /*!
        @brief Get acceleration on Z axis

        @return Acceleration on Z axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getAccZ();

    /*!
        @brief Get angular velocity of roll axis (rotation axis on X axis)

        @return Angular velocity of roll axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getGyroRoll();

    /*!
        @brief Get angular velocity of pitch axis (rotation axis on Y axis)

        @return Angular velocity of pitch axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getGyroPitch();

    /*!
        @brief Get angular velocity of yaw axis (rotation axis on Z axis)

        @return Angular velocity of yaw axis

        @attention
        Return the value cached at the runtime of the sampling method.
    */
    const int16_t& getGyroYaw();

    /*!
        @brief Dump all sensors' value after sampling

        Output result in JSON format as below.
        @code
        {
            "Acc X": <integer>,
            "Acc Y": <integer>,
            "Acc Z": <integer>,
            "Gyro Roll": <integer>,
            "Gyro Pitch": <integer>,
            "Gyro Yaw": <integer>
        }
        @endcode
    */
    void dump();
};

#endif // PLEN2_ACCELERATION_GYRO_SENSOR_H
