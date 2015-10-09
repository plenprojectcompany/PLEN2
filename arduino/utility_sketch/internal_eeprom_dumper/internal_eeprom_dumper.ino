#include <EEPROM.h>

void setup()
{
	while (!Serial); // Arduino Micro only.

	for (int address = 0; address < 1024; address++)
	{
		Serial.print(address);
		Serial.print(F(" : "));
		Serial.println(EEPROM.read(address));
	}
}

void loop()
{
	// noop.
}