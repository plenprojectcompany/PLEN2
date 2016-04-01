#include <EEPROM.h>


void setup()
{
	while (!Serial);

	Serial.begin(2000000);

	for (int address = 0; address < 1024; address++)
	{
		Serial.print(address);
		Serial.print(F(" : "));
		Serial.println(EEPROM[address]);
	}
}

void loop()
{
	// noop.
}