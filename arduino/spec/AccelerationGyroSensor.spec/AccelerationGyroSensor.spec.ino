#line 2 "AccelerationGyroSensor.spec.ino"


#include <ArduinoUnit.h>
#include "AccelerationGyroSensor.h"


#define _TEST_USER true //!< ユーザテストについても実行します。

namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


/*!
	@brief 重力軸がX軸となる条件のテスト
*/
test(GravityAxisX)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int acc_x = abs(acc_gyro.getAccX());
			int acc_y = abs(acc_gyro.getAccY());
			int acc_z = abs(acc_gyro.getAccZ());

			if (acc_x > acc_y && acc_x > acc_z)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 重力軸がY軸となる条件のテスト
*/
test(GravityAxisY)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int acc_x = abs(acc_gyro.getAccX());
			int acc_y = abs(acc_gyro.getAccY());
			int acc_z = abs(acc_gyro.getAccZ());

			if (acc_y > acc_z && acc_y > acc_x)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 重力軸がZ軸となる条件のテスト
*/
test(GravityAxisZ)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int acc_x = abs(acc_gyro.getAccX());
			int acc_y = abs(acc_gyro.getAccY());
			int acc_z = abs(acc_gyro.getAccZ());

			if (acc_z > acc_x && acc_z > acc_y)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がRoll軸となる条件のテスト
*/
test(GyroAxisRoll)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int gyro_roll  = abs(acc_gyro.getGyroRoll());
			int gyro_pitch = abs(acc_gyro.getGyroPitch());
			int gyro_yaw   = abs(acc_gyro.getGyroYaw());

			if (gyro_roll > gyro_pitch && gyro_roll > gyro_yaw)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がPitch軸となる条件のテスト
*/
test(GyroAxisPitch)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int gyro_roll  = abs(acc_gyro.getGyroRoll());
			int gyro_pitch = abs(acc_gyro.getGyroPitch());
			int gyro_yaw   = abs(acc_gyro.getGyroYaw());

			if (gyro_pitch > gyro_yaw && gyro_pitch > gyro_roll)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がYaw軸となる条件のテスト
*/
test(GyroAxisYaw)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();
			delay(50);

			int gyro_roll  = abs(acc_gyro.getGyroRoll());
			int gyro_pitch = abs(acc_gyro.getGyroPitch());
			int gyro_yaw   = abs(acc_gyro.getGyroYaw());

			if (gyro_yaw > gyro_roll && gyro_yaw > gyro_pitch)
			{
				pass();
				break;
			}
		}
	#else
		skip();
	#endif
}


/*!
	@brief 各種センサ値のダンプテスト

	ユーザによる目視でのテストです。
*/
test(Dump)
{
	#if _TEST_USER
		acc_gyro.dump();

		pass();
	#else
		skip();
	#endif
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
