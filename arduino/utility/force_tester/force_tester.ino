#include <Arduino.h>
#include <EEPROM.h>

#include "System.h"
#include "JointController.h"


namespace
{
    PLEN2::JointController joint_ctrl;

    enum { LOOP_MAX = 100 };
    uint16_t loop_count = LOOP_MAX;


    void smoothMove(int16_t begin_angle, int16_t end_angle, uint16_t duration_ms)
    {
        enum { UPDATE_INTERVAL_MS = 32 };

        const uint16_t update_count = duration_ms / UPDATE_INTERVAL_MS;
        const int16_t interval_angle = (end_angle - begin_angle) / static_cast<int16_t>(update_count);

        uint16_t count = 0;

        while (count < update_count)
        {
            joint_ctrl.setAngle(0, begin_angle + interval_angle * static_cast<int16_t>(count++));
            delay(UPDATE_INTERVAL_MS);
        }

        joint_ctrl.setAngle(0, end_angle);
    }
}


void setup()
{
    PLEN2::System::begin();

    joint_ctrl.loadSettings();
    joint_ctrl.setAngle(0, 0);

    while (!Serial);
}

void loop()
{
    while (loop_count)
    {
        Serial.print(F("Try: "));
        Serial.println(LOOP_MAX - (--loop_count));

        smoothMove(0, 700, 500);
        smoothMove(700, 0, 500);
    }

    if (loop_count == 0)
    {
        joint_ctrl.setAngle(0, 0);

        Serial.println(F("\nPlease enter any key to retry...\n"));
        while(Serial.available() == 0);
        while(Serial.available()) Serial.read();

        loop_count = LOOP_MAX;
    }
}
