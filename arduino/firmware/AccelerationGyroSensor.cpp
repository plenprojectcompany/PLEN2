/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include "Arduino.h"

#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"

#if DEBUG
	#include "Profiler.h"
#endif


namespace Shared
{
	PLEN2::System system;
}

namespace
{
	template<typename T>
	void endian_cast(T& value)
	{
		char  temp;
		char* filler = reinterpret_cast<char*>(&value);

		for (int index = 0; index < (sizeof(T) / 2); index++)
		{
			temp = filler[sizeof(T) - 1 - index];
			filler[sizeof(T) - 1 - index] = filler[index];
			filler[index] = temp;
		}
	}

	template<>
	void endian_cast(int& value)
	{
		value = ((value & 0x00FF) << 8) | ((value >> 8) & 0x00FF);
	}
}

void PLEN2::AccelerationGyroSensor::sampling()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::sampling()"));
	#endif

	/*!
		@note
		Convert data flow into "base-board -> head-board" by substituting HIGH for Pin::RS485_TXD().
		If sending any data to head-board, sensor responds values formatting 2byte, big-endian.

		Just after sending data, must convert data flow into "head-board -> base-board"
		by substituting LOW for Pin::RS485_TXD(), for receiving the values.
	*/
	digitalWrite(Pin::RS485_TXD(), HIGH);
	Shared::system.BLESerial().write('<');
	Shared::system.BLESerial().flush();

	digitalWrite(Pin::RS485_TXD(), LOW);

	char  read_count = 0;
	char* filler = reinterpret_cast<char*>(m_values);

	while (true)
	{
		if (Shared::system.BLESerial().available())
		{
			filler[read_count++] = Shared::system.BLESerial().read();
		}

		if (read_count == (SUM * sizeof(int)))
		{
			// @attention For skipping to read '\n'.
			Shared::system.BLESerial().read();

			for (int index = 0; index < SUM; index++)
			{
				endian_cast(m_values[index]);
			}

			break;
		}

		delay(1); // @attention A countermeasure of optimization.
	}
}

const int& PLEN2::AccelerationGyroSensor::getAccX()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getAccX()"));
	#endif

	return m_values[ACC_X];
}

const int& PLEN2::AccelerationGyroSensor::getAccY()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getAccY()"));
	#endif

	return m_values[ACC_Y];
}

const int& PLEN2::AccelerationGyroSensor::getAccZ()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getAccZ()"));
	#endif

	return m_values[ACC_Z];
}

const int& PLEN2::AccelerationGyroSensor::getGyroRoll()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getGyroRoll()"));
	#endif

	return m_values[GYRO_ROLL];
}

const int& PLEN2::AccelerationGyroSensor::getGyroPitch()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getGyroPitch()"));
	#endif

	return m_values[GYRO_PITCH];
}

const int& PLEN2::AccelerationGyroSensor::getGyroYaw()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::getGyroYaw()"));
	#endif

	return m_values[GYRO_YAW];
}

void PLEN2::AccelerationGyroSensor::dump()
{
	#if DEBUG
		volatile Utility::Profiler p(F("AccelerationGyroSensor::dump()"));
	#endif

	sampling();

	Shared::system.outputSerial().println(F("{"));

	Shared::system.outputSerial().print(F("\t\"Acc X\": "));
	Shared::system.outputSerial().print(getAccX());
	Shared::system.outputSerial().println(F(","));

	Shared::system.outputSerial().print(F("\t\"Acc Y\": "));
	Shared::system.outputSerial().print(getAccY());
	Shared::system.outputSerial().println(F(","));

	Shared::system.outputSerial().print(F("\t\"Acc Z\": "));
	Shared::system.outputSerial().print(getAccZ());
	Shared::system.outputSerial().println(F(","));

	Shared::system.outputSerial().print(F("\t\"Gyro Roll\": "));
	Shared::system.outputSerial().print(getGyroRoll());
	Shared::system.outputSerial().println(F(","));

	Shared::system.outputSerial().print(F("\t\"Gyro Pitch\": "));
	Shared::system.outputSerial().print(getGyroPitch());
	Shared::system.outputSerial().println(F(","));

	Shared::system.outputSerial().print(F("\t\"Gyro Yaw\": "));
	Shared::system.outputSerial().println(getGyroYaw());

	Shared::system.outputSerial().println(F("}"));
}
