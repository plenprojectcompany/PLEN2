#include <stdint.h>
#include <EEPROM.h>


void setup()
{
    while (!Serial);

    Serial.begin(2000000);

    for (uint16_t address = 0; address < 1024; address++)
    {
        Serial.print(address);
        Serial.print(F(" : "));
        Serial.println(EEPROM[address], HEX);
    }
}

void loop()
{
    // no operations.
}