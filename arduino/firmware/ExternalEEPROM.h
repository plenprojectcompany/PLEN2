/*!
	@file      ExternalEEPROM.h
	@brief     Management class of external EEPROM.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_EXTERNAL_EEPROM_H
#define PLEN2_EXTERNAL_EEPROM_H

namespace PLEN2
{
	class ExternalEEPROM;
}

/*!
	@brief Management class of external EEPROM

	@note
	24FC1025 supports to access 128 bytes at once, but Arduino's I2C library is not supported the method
	because the library's buffer size is 32 bytes.
	<br><br>
	Please pay attention to it is including bytes of targeted area address (= 2 bytes).
	Accurate data size are 30 bytes that you can write.
	(This is the reason that there are differences between CHUNK_SIZE() and SLOT_SIZE().)
*/
class PLEN2::ExternalEEPROM
{
private:
	//! @brief Communication clock of I2C interface
	inline static const long CLOCK()         { return 400000L; }

	//! @brief Size of external EEPROM (bytes)
	inline static const long SIZE()          { return 0x20000L; }

	//! @brief Device address on I2C network
	inline static const int  ADDRESS()       { return 0x50; }

	//! @brief Bytes of targeted area address
	inline static const int  ADDRESS_BYTES() { return 2; }

	//! @brief Selection bit of memory chip
	inline static const int  SELECT_BIT()    { return 2; }

public:
	//! @brief Chunk size of external EEPROM (bytes)
	inline static const int CHUNK_SIZE() { return 32; }

	//! @brief Slot size of external EEPROM (bytes)
	inline static const int SLOT_SIZE()  { return (CHUNK_SIZE() - ADDRESS_BYTES()); }

	//! @brief Beginning value of slots
	inline static const int SLOT_BEGIN() { return 0; }

	//! @brief End value of slots
	inline static const int SLOT_END()   { return SIZE() / CHUNK_SIZE(); }

	/*!
		@brief Constructor
	*/
	ExternalEEPROM();

	/*!
		@brief Read a slot of external EEPROM

		@param [in]  slot      Please set slot number you want to read.
		@param [out] data[]    Please set buffer to store reading data.
		@param [in]  read_size Please set buffer size.

		@return Result
		@retval !0 Succeeded. (Generally, the value equals **read-size**.)
		@retval -1 Failed.
	*/
	static char readSlot(unsigned int slot, char data[], unsigned char read_size);

	/*!
		@brief Write a slot of external EEPROM

		@param [in] slot       Please set slot number you want to write.
		@param [in] data[]     Please set buffer that stored writing data.
		@param [in] write_size Please set buffer size.

		@return Result
		@retval 0  Succeeded.
		@retval -1 Argument error. (**write_size** is bigger than slot size.)
		@retval 1  Sending-buffer overflow.
		@retval 2  Received NACK after sending slave address.
		@retval 3  Received NACK after sending data bytes.
		@retval 4  Other errors were raised.

		@attention
		Writing external EEPROM requires time. (Typically using 3[msec].)
		In the implementation, 5[msec] delay is inserted at end of the method.
	*/
	static char writeSlot(unsigned int slot, const char data[], unsigned char write_size);
};

#endif // PLEN2_EXTERNAL_EEPROM_H