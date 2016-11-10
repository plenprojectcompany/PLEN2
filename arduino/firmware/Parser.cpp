/*
    Copyright (c) 2015,
    - Kazuyuki TAKASE - https://github.com/Guvalif
    - PLEN Project Company Inc. - https://plen.jp

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
    // no operations.
}

AbstractParser::~AbstractParser()
{
    // no operations.
}

const int8_t& AbstractParser::index()
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
    // no operations.
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
    // no operations.
}

CharGroupParser::~CharGroupParser()
{
    // no operations.
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
StringGroupParser::StringGroupParser(const char* accept_strs[], const uint8_t size)
    : m_accept_strs(accept_strs)
    , m_size(size)
{
    // no operations.
}

StringGroupParser::~StringGroupParser()
{
    // no operations.
}

bool StringGroupParser::parse(const char* input)
{
    uint8_t begin  = 0;
    uint8_t middle = m_size / 2;
    uint8_t end    = m_size;

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
    // no operations.
}

HexStringParser::~HexStringParser()
{
    // no operations.
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
    @brief Convert hex string to an uint16_t
*/
uint16_t hexbytes2uint16_impl(const char* bytes, uint8_t size)
{
    uint16_t result = 0;

    for (uint8_t index = 0; index < size; index++)
    {
        uint16_t placeholder = bytes[index];

        if (placeholder >= 'a') placeholder -= ('a' - 10);
        if (placeholder >= 'A') placeholder -= ('A' - 10);
        if (placeholder >= '0') placeholder -= '0';

        uint16_t base = 0x01 << ((size - index - 1) * 4);

        result += placeholder * base;
    }

    return result;
}


/*!
    @brief Convert hex string to an int16_t
*/
int16_t hexbytes2int16_impl(const char* bytes, uint8_t size)
{
    uint16_t temp = hexbytes2uint16_impl(bytes, size);

    temp <<= (((sizeof(int16_t) * 2) - size) * 4);

    /*!
        @note
        The lines expressed by (*) are not necessary because avr-gcc supports arithmetic shifts.
    */
    int16_t result = temp;
    // bool negative = (result < 0); // (*)

    result >>= (((sizeof(int16_t) * 2) - size) * 4);
    // if (negative) result |= (0xFFFF << (size * 4)); // (*)

    return result;
}

} // end of namespace "Utility".