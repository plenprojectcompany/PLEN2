/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

    This software is released under the MIT License.
    (See also : http://opensource.org/licenses/mit-license.php)
*/

#include <Arduino.h>

#include "Profiler.h"


namespace
{
    namespace Shared
    {
        uint16_t m_nest = 0;
    }
}


void Utility::Profiler::m_tabbing()
{
    for (uint16_t i = 0; i < Shared::m_nest; i++)
    {
        Serial.write('\t');
    }
}


Utility::Profiler::Profiler(const __FlashStringHelper* fsh_ptr)
{
    m_tabbing();
    Serial.print(F(">>> pushed : "));
    Serial.println(fsh_ptr);

    m_tabbing();
    Serial.print(F("+++ stack ptr : "));
    Serial.println(reinterpret_cast<uint16_t>(this), HEX);

    Shared::m_nest++;
    m_begin = micros();
}


Utility::Profiler::~Profiler()
{
    m_end = micros();
    Shared::m_nest--;

    m_tabbing();
    Serial.print(F("+++ nest      : "));
    Serial.println(Shared::m_nest);

    m_tabbing();
    Serial.print(F("+++ exec time : "));
    Serial.print(m_end - m_begin);
    Serial.println(F(" [usec]"));

    m_tabbing();
    Serial.println(F("<<< popped"));
}
