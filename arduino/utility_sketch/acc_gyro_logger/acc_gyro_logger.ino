#include "AccelerationGyroSensor.h"


namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


void setup()
{
	while (!Serial);
}

void loop()
{
	/*!
		@attention
		BLE基板に電力供給が完了するのを待つ必要があるため、
		delay()を各種メソッドの呼び出し前に実行する必要がある。
		(setup()でいくらかdelay()を設定するでも可。)
	*/
	delay(100);

	unsigned long begin = micros();
	acc_gyro.sampling();
	unsigned long end   = micros();

	Serial.print(F("# exec time : AccelerationGyroSensor::sampling() = "));
	Serial.println(end - begin);

	acc_gyro.dump();
}
