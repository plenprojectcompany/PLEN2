/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include "Arduino.h"
#include <Wire.h>

#include "ExternalEEPROM.h"

#if DEBUG
	#include "System.h"
	#include "Profiler.h"
#endif


PLEN2::ExternalEEPROM::ExternalEEPROM()
{
	Wire.begin();
	Wire.setClock(CLOCK());
}


char PLEN2::ExternalEEPROM::readSlot(
	unsigned int  slot,
	char          data[],
	unsigned char read_size
)
{
	#if DEBUG
		volatile Utility::Profiler p(F("ExternalEEPROM::readSlot()"));
	#endif

	if (   (slot >= SLOT_END())
		|| (read_size > SLOT_SIZE())
	)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argument! : slot = "));
			System::debugSerial().print(slot);
			System::debugSerial().print(F(", or read_size = "));
			System::debugSerial().println(read_size);
		#endif

		return -1;
	}


	int slave_address = ADDRESS();
	unsigned long data_address = static_cast<unsigned long>(slot) * CHUNK_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // Select the memory block B0 = 1.
		data_address  -= (SIZE() / 2);
	}

	#if DEBUG
		System::debugSerial().print(F(">>> slave_address = "));
		System::debugSerial().println(slave_address, HEX);

		System::debugSerial().print(F(">>> data_address = "));
		System::debugSerial().println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // Sending targeted address's high byte.
	Wire.write((byte)(data_address & 0x00FF)); // Sending targeted address's low byte.

	if (Wire.endTransmission() == 0)
	{
		if (Wire.requestFrom(slave_address, read_size) == read_size)
		{
			for (char index = 0; index < read_size; index++)
			{
				data[index] = Wire.read();
			}

			return read_size;
		}
	}

	return -1;
}


char PLEN2::ExternalEEPROM::writeSlot(
	unsigned int  slot,
	const char    data[],
	unsigned char write_size
)
{
	#if DEBUG
		volatile Utility::Profiler p(F("ExternalEEPROM::writeSlot()"));
	#endif

	if (   (slot >= SLOT_END())
		|| (write_size > SLOT_SIZE())
	)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argument! : slot = "));
			System::debugSerial().print(slot);
			System::debugSerial().print(F(", or write_size = "));
			System::debugSerial().println(write_size);
		#endif

		return -1;
	}


	int slave_address = ADDRESS();
	unsigned long data_address = static_cast<unsigned long>(slot) * CHUNK_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // Select the memory block B0 = 1.
		data_address  -= (SIZE() / 2);
	}

	#if DEBUG
		System::debugSerial().print(F(">>> slave_address : "));
		System::debugSerial().println(slave_address, HEX);

		System::debugSerial().print(F(">>> data_address : "));
		System::debugSerial().println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // Sending targeted address's high byte.
	Wire.write((byte)(data_address & 0x00ff)); // Sending targeted address's low byte.

	for (char index = 0; index < write_size; index++)
	{
		Wire.write(data[index]);
	}

	char ret = Wire.endTransmission();

	delay(5); // @attention Wait for writing EEPROM.

	return ret;
}
