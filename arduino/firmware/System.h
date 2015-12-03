/*!
	@file      System.h
	@brief     Management class of basis about AVR MCU.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_SYSTEM_H
#define PLEN2_SYSTEM_H


class Stream;
class __FlashStringHelper;


namespace PLEN2
{
	class System;
}

/*!
	@brief Management class of basis about AVR MCU
*/
class PLEN2::System
{
private:
	//! @brief Device name
	inline static const __FlashStringHelper* DEVICE()   { return F("PLEN2");   }

	//! @brief Code name of the firmware
	inline static const __FlashStringHelper* CODENAME() { return F("Cytisus"); }

	//! @brief Version number of the firmware
	inline static const __FlashStringHelper* VERSION()  { return F("1.3.0");   }

	//! @brief Communication speed of USB serial
	inline static const long USBSERIAL_BAUDRATE() { return 2000000L; }
	
	//! @brief Communication speed of BLE serial
	inline static const long BLESERIAL_BAUDRATE() { return 2000000L; }

public:
	//! @brief Size of internal eeprom
	inline static const int INTERNAL_EEPROMSIZE() { return 1024; }

	/*!
		@brief Constructor
	*/
	System();

	/*!
		@brief Get USB-serial instance

		@return Reference of USB-serial instance
	*/
	static Stream& USBSerial();

	/*!
		@brief Get BLE-serial instance

		@return Reference of BLE-serial instance
	*/
	static Stream& BLESerial();

	/*!
		@brief Get input-serial instance

		@return Reference of input-serial instance
	*/
	static Stream& inputSerial();

	/*!
		@brief Get output-serial instance
		
		@return Reference of output-serial instance
	*/
	static Stream& outputSerial();

	/*!
		@brief Get debug-serial instance
		
		@return Reference of debug-serial instance
	*/
	static Stream& debugSerial();

	/*!
		@brief Dump information of the system

		Outputs result like JSON format below.
		@code
		{
			"device": <string>,
			"codename": <string>,
			"version": <string>
		}
		@endcode
	*/
	static void dump();
};

#endif // PLEN2_SYSTEM_H