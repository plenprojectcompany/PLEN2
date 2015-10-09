#include "System.h"


namespace
{
	PLEN2::System system;
}


void setup()
{
	// noop.
}

void loop()
{
	if (system.USBSerial().available())
	{
		system.USBSerial().write(system.USBSerial().read());
	}

	if (system.BLESerial().available())
	{
		system.USBSerial().write(system.BLESerial().read());
	}
}