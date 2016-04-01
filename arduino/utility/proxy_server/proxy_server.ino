#include <Arduino.h>

#include "Pin.h"
#include "System.h"


void setup()
{
	volatile PLEN2::System s;

	digitalWrite(PLEN2::Pin::RS485_TXD(), HIGH);
}

void loop()
{
	using namespace PLEN2;

	if (System::USBSerial().available())
	{
		System::BLESerial().write(System::USBSerial().read());
	}
}
