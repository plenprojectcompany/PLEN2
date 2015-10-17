/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

// Arduinoライブラリ
#include "Arduino.h"
#include <Wire.h>

// 独自ライブラリ
#if _DEBUG
	#include "System.h"
#endif
#include "ExternalEEPROM.h"


namespace {
	#if _DEBUG
		PLEN2::System system;
	#endif
}


PLEN2::ExternalEEPROM::ExternalEEPROM()
{
	Wire.begin();
	Wire.setClock(CLOCK());
}


int PLEN2::ExternalEEPROM::readSlot(
	unsigned int slot,
	char         data[],
	unsigned int read_size
)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : ExternalEEPROM::readSlot()"));
	#endif

	if (   (slot >= SLOT_END())
		|| (read_size > SLOT_SIZE())
	)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argument! : slot = "));
			system.outputSerial().print(slot);
			system.outputSerial().print(F(", or read_size = "));
			system.outputSerial().println(read_size);
		#endif

		return -1;
	}

	int  slave_address = ADDRESS();
	long data_address  = (long)slot * CHUNK_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
		data_address  -= (SIZE() / 2);
	}

	#if _DEBUG
		system.outputSerial().print(F(">>> slave_address = "));
		system.outputSerial().println(slave_address, HEX);
		system.outputSerial().print(F(">>> data_address = "));
		system.outputSerial().println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // High側アドレスを指定
	Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定
	int ret = Wire.endTransmission();

	if (ret == 0)
	{
		ret = Wire.requestFrom(slave_address, read_size);
		if (ret == read_size)
		{
			for (int index = 0; index < read_size; index++)
			{
				data[index] = Wire.read();
			}
		}
		else
		{
			ret = -1;
		}
	}
	else
	{
		ret = - 1;
	}

	return ret;
}


int PLEN2::ExternalEEPROM::writeSlot(
	unsigned int slot,
	const char   data[],
	unsigned int write_size
)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : ExternalEEPROM::writeSlot()"));
	#endif

	if (   (slot >= SLOT_END())
		|| (write_size > SLOT_SIZE())
	)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argument! : slot = "));
			system.outputSerial().print(slot);
			system.outputSerial().print(F(", or write_size = "));
			system.outputSerial().println(write_size);
		#endif
		
		return -1;
	}

	int  slave_address = ADDRESS();
	long data_address  = (long)slot * CHUNK_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
		data_address  -= (SIZE() / 2);
	}

	#if _DEBUG
		system.outputSerial().print(F(">>> slave_address : "));
		system.outputSerial().println(slave_address, HEX);
		system.outputSerial().print(F(">>> data_address : "));
		system.outputSerial().println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // High側アドレスを指定
	Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定

	for (int index = 0; index < write_size; index++)
	{
		Wire.write(data[index]);
	}

	int ret = Wire.endTransmission();
	delay(5);

	return ret;
}
