#include "AccelerationGyroSensor.h"
#include "Profiler.h"


namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


void setup()
{
	while (!Serial);

	delay(3000); //!< @attention BLEチップのファームウェア起動を待つ。
}

void loop()
{
	{
		volatile Utility::Profiler p(F("AccelerationGyroSensor::sampling()"));

		acc_gyro.sampling();
	}

	acc_gyro.dump();
}
