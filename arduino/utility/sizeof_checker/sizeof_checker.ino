#include <stdint.h>


void setup()
{
    Serial.begin(2000000);

    while (!Serial); // Arduino Micro only.

    Serial.print(F("char  : "));
    Serial.print(sizeof(char));
    Serial.println(F(" byte"));

    Serial.print(F("int   : "));
    Serial.print(sizeof(int));
    Serial.println(F(" byte"));

    Serial.print(F("bool  : "));
    Serial.print(sizeof(bool));
    Serial.println(F(" byte"));

    Serial.print(F("short : "));
    Serial.print(sizeof(short));
    Serial.println(F(" byte"));

    Serial.print(F("long  : "));
    Serial.print(sizeof(long));
    Serial.println(F(" byte"));

    Serial.print(F("float : "));
    Serial.print(sizeof(float));
    Serial.println(F(" byte"));

    uint16_t endianness = 0x01;
    const uint8_t* scanner = reinterpret_cast<const uint8_t*>(&endianness);

    Serial.print(F("Endianness : "));
    Serial.println((scanner[0])? F("little") : F("big"));
}


void loop()
{
    // noop.
}
