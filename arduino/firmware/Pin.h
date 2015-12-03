/*!
	@file      Pin.h
	@brief     Management namespace of pin mapping.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_PIN_H
#define PLEN2_PIN_H

namespace PLEN2
{
	/*!
		@brief Management namespace of pin mapping

		Please give the standard Arduino libraries the methods returning values.
		The methods are evaluated at compile time, so there is no overhead at runtime.

		@note
		It helps your understanding that to refer the PLEN2's circuit and schematic.
		-> https://github.com/plenproject/plen__baseboard

		@sa
		Arduino Micro's pin mapping -> http://arduino.cc/en/Hacking/PinMapping32u4
	 */
	namespace Pin
	{
		//! @brief Output selection bit of multiplexer (0bit)
		inline static const int MULTIPLEXER_SELECT0() { return 5;  }

		//! @brief Output selection bit of multiplexer (1bit)
		inline static const int MULTIPLEXER_SELECT1() { return 6;  }

		//! @brief Output selection bit of multiplexer (2bit)
		inline static const int MULTIPLEXER_SELECT2() { return 12; }

		//! @brief Output of PWM, for servo 00 to 07
		inline static const int PWM_OUT_00_07()       { return 11; }

		//! @brief Output of PWM, for servo 08 to 15
		inline static const int PWM_OUT_08_15()       { return 10; }

		//! @brief Output of PWM, for servo 16 to 23
		inline static const int PWM_OUT_16_23()       { return 9;  }

		//! @brief Data flow selection bit of serial
		inline static const int RS485_TXD()           { return 4;  }

		//! @brief Output of LED
		inline static const int LED_OUT()             { return 13; }

		//! @brief Input of random-device (Get an open circuit voltage.)
		inline static const int RANDOM_DEVICE_IN()    { return 6;  }
	}
}

#endif // PLEN2_PIN_H