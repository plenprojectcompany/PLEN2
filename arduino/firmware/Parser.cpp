/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <ctype.h>
#include <string.h>

#include "Parser.h"


namespace Utility
{

/*!
	@brief Abstract parser interface
*/
AbstractParser::AbstractParser()
	: m_index(-1)
{
	// noop.
}

AbstractParser::~AbstractParser()
{
	// noop.
}

const char& AbstractParser::index()
{
	return m_index;
}


/*!
	@brief Parser class that accepts all
*/
NilParser::NilParser()
{
	m_index = 0;
}

NilParser::~NilParser()
{
	// noop.
}

bool NilParser::parse(const char* input)
{
	return true;
}


/*!
	@brief Parser class that accepts only characters given
*/
CharGroupParser::CharGroupParser(const char* accept_chars)
	: m_accept_chars(accept_chars)
{
	// noop.
}

CharGroupParser::~CharGroupParser()
{
	// noop.
}

bool CharGroupParser::parse(const char* input)
{
	const char* it = m_accept_chars;

	m_index = 0;
	while (it[m_index] != '\0')
	{
		if (it[m_index] == input[0])
		{
			return true;
		}

		m_index++;
	}

	m_index = -1;
	return false;
}


/*!
	@brief Parser class that accepts only strings given
*/
StringGroupParser::StringGroupParser(const char* accept_strs[], const unsigned char size)
	: m_accept_strs(accept_strs)
	, m_size(size)
{
	// noop.
}

StringGroupParser::~StringGroupParser()
{
	// noop.
}

bool StringGroupParser::parse(const char* input)
{
	unsigned char begin  = 0;
	unsigned char middle = m_size / 2;
	unsigned char end    = m_size;

	while (begin <= end)
	{
		if (strlen(input) != strlen(m_accept_strs[middle]))
		{
			m_index = -1;
			return false;
		}

		int result = strcasecmp(input, m_accept_strs[middle]);

		if (result == 0)
		{
			m_index = middle;
			return true;
		}

		if (result > 0)
		{
			begin  = middle + 1;
			middle = (begin + end) / 2;

			continue;
		}

		if (result < 0)
		{
			end    = middle - 1;
			middle = (begin + end) / 2;

			continue;
		}
	}

	m_index = -1;
	return false;
}


/*!
	@brief Parser class that accepts only hex string
*/
HexStringParser::HexStringParser()
{
	// noop.
}

HexStringParser::~HexStringParser()
{
	// noop.
}

bool HexStringParser::parse(const char* input)
{
	do
	{
		if (isxdigit(*input++) == 0)
		{
			m_index = -1;
			return false;
		}
	} while (*input != '\0');

	m_index = 0;
	return true;
}


/*!
	@brief Convert hex string to an unsigned int
*/
unsigned int hexbytes2uint(const char* bytes, unsigned char size)
{
	unsigned int result = 0;

	for (char index = 0; index < size; index++)
	{
		unsigned int placeholder = bytes[index];

		if (placeholder >= 'a') placeholder -= ('a' - 10);
		if (placeholder >= 'A') placeholder -= ('A' - 10);
		if (placeholder >= '0') placeholder -= '0';

		unsigned int base = 0x01 << ((size - index - 1) * 4);

		result += placeholder * base;
	}

	return result;
}


/*!
	@brief Convert hex string to an int
*/
int hexbytes2int(const char* bytes, unsigned char size)
{
	unsigned int temp = hexbytes2uint(bytes, size);

	temp <<= (((sizeof(int) * 2) - size) * 4);

	/*!
		@note
		The lines expressed by (*) are not necessary because avr-gcc supports arithmetic shifts.
	*/
	int result = temp;
	// bool negative = (result < 0); // (*)

	result >>= (((sizeof(int) * 2) - size) * 4);
	// if (negative) result |= (0xFFFF << (size * 4)); // (*)

	return result;
}

} // end of namespace "Utility".