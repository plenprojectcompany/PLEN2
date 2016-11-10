#include <Arduino.h>

#include "System.h"


void setup()
{
    volatile PLEN2::System s;
}

void loop()
{
    using namespace PLEN2;

    if (System::USBSerial().available())
    {
        System::USBSerial().write( System::USBSerial().read() );
    }

    if (System::BLESerial().available())
    {
        System::USBSerial().write( System::BLESerial().read() );
    }
}