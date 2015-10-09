/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

// 標準ライブラリ
#include <ctype.h>
#include <string.h>

// 独自ライブラリ
#include "Purser.h"


namespace Utility
{

/*!
	@brief 抽象パーサクラス(インタフェース)
*/
AbstractPurser::AbstractPurser()
	: m_index(-1)
{
	// noop.
}

AbstractPurser::~AbstractPurser()
{
	// noop.
}

const char& AbstractPurser::index()
{
	return m_index;
}


/*!
	@brief 常に受理するパーサクラス
*/
NilPurser::NilPurser()
{
	// noop.
}

NilPurser::~NilPurser()
{
	// noop.
}

bool NilPurser::purse(const char* input)
{
	m_index = 0;
	return true;
}


/*!
	@brief 与えられた文字グループのみ受理するパーサクラス
*/
CharGroupPurser::CharGroupPurser(const char* accept_chars)
	: m_accept_chars(accept_chars)
{
	// noop.
}

CharGroupPurser::~CharGroupPurser()
{
	// noop.
}

bool CharGroupPurser::purse(const char* input)
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
	@brief 与えられた文字列グループのみ受理するパーサクラス
*/
StringGroupPurser::StringGroupPurser(const char* accept_strs[], const unsigned char size)
	: m_accept_strs(accept_strs)
	, m_size(size)
{
	// noop.
}

StringGroupPurser::~StringGroupPurser()
{
	// noop.
}

bool StringGroupPurser::purse(const char* input)
{
	int begin  = 0;
	int middle = m_size / 2;
	int end    = m_size;

	while (begin <= end)
	{
		int result = strncasecmp(input, m_accept_strs[middle], 2);

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
	@brief 16進文字列のみ受理するパーサクラス
*/
HexStringPurser::HexStringPurser()
{
	// noop.
}

HexStringPurser::~HexStringPurser()
{
	// noop.
}

bool HexStringPurser::purse(const char* input)
{
	const char* it = input;

	while (*it != '\0')
	{
		if (isxdigit(*it++) == 0)
		{
			m_index = -1;
			return false;
		}
	}

	m_index = 0;
	return true;
}

} // end of namespace "Utility".