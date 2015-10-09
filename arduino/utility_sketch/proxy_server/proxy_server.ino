#include "Pin.h"
#include "System.h"


namespace
{
	PLEN2::System system;
}


void setup()
{
	digitalWrite(PLEN2::Pin::RS485_TXD(), HIGH);
}

void loop()
{
	if (system.USBSerial().available())
	{
		system.BLESerial().write(system.USBSerial().read());
	}
}
