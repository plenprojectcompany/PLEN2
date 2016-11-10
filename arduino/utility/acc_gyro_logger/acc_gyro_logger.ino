#include <Arduino.h>

#include "System.h"
#include "AccelerationGyroSensor.h"


namespace
{
    PLEN2::AccelerationGyroSensor acc_gyro;

    uint32_t call_count = 1;
}


void setup()
{
    PLEN2::System::begin();

    while (!Serial);

    delay(3000); //!< @attention A wait for starting up BLE MCU's firmware
}

void loop()
{
    PLEN2::System::outputSerial().print(acc_gyro.sampling() ? F("OK : ") : F("NG : "));
    PLEN2::System::outputSerial().println(call_count++);

    acc_gyro.dump();

    delay(100);
}
