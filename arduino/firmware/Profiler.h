/*!
    @file      Profiler.h
    @brief     Tiny metrics class for Arduino.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef UTILITY_PROFILER_H
#define UTILITY_PROFILER_H


#include <stdint.h>

class __FlashStringHelper;


/*!
    @brief Sugar syntax for the profiler

    @attention
    You shouldn't use the macro with `__func__` macro,
    because `__func__` macro is expanded as static const char pointer.
*/
#define PROFILING(FUNC_NAME) volatile Utility::Profiler p(F(FUNC_NAME))


namespace Utility
{
    class Profiler;
}

/*!
    @brief Tiny metrics class

    Refer to the usage below.
    @code
    void anyFunction()
    {
        // When instantiating the class, it outputs a given message.
        volatile Utility::Profiler p(F("anyFunction()"));

        Any code here...

        // Outputting metrics about this function when profiler instance has destroyed.
    }
    @endcode
*/
class Utility::Profiler
{
private:
    uint32_t m_begin;
    uint32_t m_end;

    void m_tabbing();

    // Disable copy constructor and operator =.
    Profiler(const Profiler&);
    Profiler& operator=(const Profiler&);

public:
    /*!
        @brief Constructor

        @param [in] fsh_ptr Please set startup message.

        @attention
        Arduino IDE is using optimization option -Os,
        so you should use volatile prefix when instantiate the class.
    */
    Profiler(const __FlashStringHelper* fsh_ptr);

    /*!
        @brief Destructor
    */
    ~Profiler();
};

#endif // UTILITY_PROFILER_H
