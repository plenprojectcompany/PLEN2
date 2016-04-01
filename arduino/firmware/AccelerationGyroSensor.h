/*!
	@file      AccelerationGyroSensor.h
	@brief     Management class of acceleration and gyro sensor.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_ACCELERATION_GYRO_SENSOR_H
#define PLEN2_ACCELERATION_GYRO_SENSOR_H

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
	enum SENSOR_VALUE_MAP {
		ACC_X,
		ACC_Y,
		ACC_Z,
		GYRO_ROLL,
		GYRO_PITCH,
		GYRO_YAW,
		SUM //!< Summation of sensors
	};

	int m_values[SUM];

public:
	/*!
		@brief Do sampling sensor values

		Usage assumption is to call the method at stated periods from loop().

		@attention
		The method has a deadlock of interruption and communication wait
		because it to communicate with the sensor through a bus,
		so using in constructor and interruption vector is deprecated.
		<br><br>
		The order of power supplied or firmware startup timing is base-board, head-board.
		If the method calls from early timing, program freezes because synchronism of communication is missed.
		(Generally, it is going to success setup() inserts 3000[msec] delays.)
	*/
	void sampling();

	/*!
		@brief Get X axis's acceleration

		@return X axis's acceleration

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getAccX();

	/*!
		@brief Get Y axis's acceleration

		@return Y axis's acceleration

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getAccY();

	/*!
		@brief Get Z axis's acceleration

		@return Z axis's acceleration

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getAccZ();

	/*!
		@brief Get roll (rotation axis on X axis) axis's angular velocity

		@return Roll axis's angular velocity

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getGyroRoll();

	/*!
		@brief Get pitch (rotation axis on Y axis) axis's angular velocity

		@return Pitch axis's angular velocity

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getGyroPitch();

	/*!
		@brief Get yaw (rotation axis on Z axis) axis's angular velocity

		@return Yaw axis's angular velocity

		@attention
		Return cached value at run the sampling method.
	*/
	const int& getGyroYaw();

	/*!
		@brief Dump all sensor values after sampling

		Outputs result like JSON format below.
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
