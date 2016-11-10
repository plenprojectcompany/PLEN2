#include <Arduino.h>
#include <EEPROM.h>

#include "System.h"
#include "JointController.h"


namespace
{
    PLEN2::JointController joint_ctrl;

    uint16_t pwm = 512;
}


void setup()
{
    PLEN2::System::begin();
    joint_ctrl.loadSettings();
}

void loop()
{
    using namespace PLEN2;

    if (System::USBSerial().available())
    {
        switch (System::USBSerial().read())
        {
            case 'm':
            {
                if (pwm != 1023)
                {
                    pwm++;
                }

                break;
            }

            case 'p':
            {
                if (pwm != 0)
                {
                    pwm--;
                }

                break;
            }

            default:
            {
                // no operations.

                break;
            }
        }

        System::USBSerial().print(F("output : "));
        System::USBSerial().print(1023 - pwm);
        System::USBSerial().print(F(" (internal = "));
        System::USBSerial().print(pwm);
        System::USBSerial().println(F(")"));

        joint_ctrl.m_pwms[0] = pwm;
    }
}
