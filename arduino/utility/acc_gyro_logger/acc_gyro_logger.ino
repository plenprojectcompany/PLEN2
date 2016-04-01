#include <Arduino.h>

#include "System.h"
#include "AccelerationGyroSensor.h"


namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


void setup()
{
	volatile PLEN2::System s;

	while (!Serial);

	delay(3000); //!< @attention A wait for starting up BLE MCU's firmware
}

void loop()
{
	acc_gyro.sampling();
	acc_gyro.dump();

	delay(500);
}
