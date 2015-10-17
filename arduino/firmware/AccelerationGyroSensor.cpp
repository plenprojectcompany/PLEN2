/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

// Arduinoライブラリ
#include "Arduino.h"

// 独自ライブラリ
#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"

#define _DEBUG false


namespace
{
	PLEN2::System system;

	enum {
		ACC_X,
		ACC_Y,
		ACC_Z,
		GYRO_ROLL,
		GYRO_PITCH,
		GYRO_YAW
	};

	template<typename T>
	void endianCast(T& value)
	{
		char  temp;
		char* filler = (char*)&value;

		for (int index = 0; index < (sizeof(T) / 2); index++)
		{
			temp = filler[sizeof(T) - 1 - index];
			filler[sizeof(T) - 1 - index] = filler[index];
			filler[index] = temp;
		}
	}
}

void PLEN2::AccelerationGyroSensor::sampling()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::sampling()"));
	#endif

	/*!
		@note
		Pin::RS485_TXD()をHIGHにすることで、データの流れを"サーボ基盤"→"頭基板"と変更する。
		頭基板のシリアルに任意のデータを送ると、2byte, ビッグエンディアンのバイト列で
		各センサ値を返却してくる。

		これを受信するために、データの流れを即座に"頭基板"→"サーボ基盤"と変更する必要がある。
	*/
	digitalWrite(Pin::RS485_TXD(), HIGH);
	system.BLESerial().write('<');
	system.BLESerial().flush();

	digitalWrite(Pin::RS485_TXD(), LOW);
	char  read_count = 0;
	char* filler = (char*)m_values;

	while (true)
	{
		if (system.BLESerial().available())
		{
			filler[read_count++] = system.BLESerial().read();
		}

		if (read_count == (SUM() * sizeof(int)))
		{
			// @attention '\n'の読み飛ばしのために必須
			system.BLESerial().read();

			for (int index = 0; index < SUM(); index++)
			{
				endianCast(m_values[index]);
			}

			break;
		}

		delay(1); // @attention 最適化対策と割り込みの疑似均等割り付けのために必須
	}
}

const int& PLEN2::AccelerationGyroSensor::getAccX()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getAccX()"));
	#endif

	return m_values[ACC_X];
}

const int& PLEN2::AccelerationGyroSensor::getAccY()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getAccY()"));
	#endif

	return m_values[ACC_Y];
}

const int& PLEN2::AccelerationGyroSensor::getAccZ()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getAccZ()"));
	#endif

	return m_values[ACC_Z];
}

const int& PLEN2::AccelerationGyroSensor::getGyroRoll()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getGyroRoll()"));
	#endif

	return m_values[GYRO_ROLL];
}

const int& PLEN2::AccelerationGyroSensor::getGyroPitch()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getGyroPitch()"));
	#endif

	return m_values[GYRO_PITCH];
}

const int& PLEN2::AccelerationGyroSensor::getGyroYaw()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::getGyroYaw()"));
	#endif

	return m_values[GYRO_YAW];
}

void PLEN2::AccelerationGyroSensor::dump()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : AccelerationGyroSensor::dump()"));
	#endif

	sampling();

	system.outputSerial().println(F("{"));

		system.outputSerial().print(F("\t\"Acc X\": "));
		system.outputSerial().print(getAccX());
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"Acc Y\": "));
		system.outputSerial().print(getAccY());
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"Acc Z\": "));
		system.outputSerial().print(getAccZ());
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"Gyro Roll\": "));
		system.outputSerial().print(getGyroRoll());
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"Gyro Pitch\": "));
		system.outputSerial().print(getGyroPitch());
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"Gyro Yaw\": "));
		system.outputSerial().println(getGyroYaw());

	system.outputSerial().println(F("}"));
}
