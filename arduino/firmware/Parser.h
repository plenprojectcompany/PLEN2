/*!
    @file      Parser.h
    @brief     Provide parser utilities.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef UTILITY_PARSER_H
#define UTILITY_PARSER_H


#include <stdint.h>

namespace Utility
{
    class AbstractParser;
    class NilParser;
    class CharGroupParser;
    class StringGroupParser;
    class HexStringParser;


    /*!
        @brief Convert hex string to an uint16_t

        @param [in] bytes Pointer of hex string buffer.
        @param [in] size  Length of hex string buffer.

        @attention
        The method does not validate its arguments.
    */
    uint16_t hexbytes2uint16_impl(const char* bytes, uint8_t size);

    /*!
        @brief hexbytes2uint16_impl with compile time assertion

        @tparam     SIZE  Length of hex string buffer.
        @param [in] bytes Pointer of hex string buffer.
    */
    template<const int SIZE>
    uint16_t hexbytes2uint16(const char* bytes)
    {
        typedef uint8_t SIZE_needs_to_be_4_and_under[(SIZE > 4)? -1 : 1];

        return hexbytes2uint16_impl(bytes, SIZE);
    }


    /*!
        @brief Convert hex string to an int16_t

        Convert with regard to the highest bit as signed bit after converting hex string to bit-expression.

        @param [in] bytes Pointer of hex string buffer.
        @param [in] size  Length of hex string buffer.

        @attention
        The method does not validate arguments.
    */
    int16_t hexbytes2int16_impl(const char* bytes, uint8_t size);

    /*!
        @brief hexbytes2int16_impl with compile time assertion.

        @tparam     SIZE  Length of hex string buffer.
        @param [in] bytes Pointer of hex string buffer.
    */
    template<const int SIZE>
    int16_t hexbytes2int16(const char* bytes)
    {
        typedef uint8_t SIZE_needs_to_be_4_and_under[(SIZE > 4)? -1 : 1];

        return hexbytes2int16_impl(bytes, SIZE);
    }
}


/*!
    @brief Abstract parser interface
*/
class Utility::AbstractParser
{
protected:
    int8_t m_index;

    /*!
        @brief Constructor

        The method is protected to disable instantiation.
    */
    AbstractParser();

public:
    /*!
        @brief Destructor
    */
    virtual ~AbstractParser();

    /*!
        @brief Parser function interface

        @param [in] input String you want to parse.

        @return Result
    */
    virtual bool parse(const char* input) = 0;

    /*!
        @brief Get matched index of after parsing

        @return Matched index
        @retval !-1 Matched. (The value is the index selected from the condition given when instantiating the class.)
        @retval -1  Parsing failed, so there is no matched index.
    */
    virtual const int8_t& index();
};


/*!
    @brief Parser class that accepts all
*/
class Utility::NilParser : public Utility::AbstractParser
{
public:
    /*!
        @brief Constructor
    */
    NilParser();

    /*!
        @brief Destructor
    */
    virtual ~NilParser();

    /*!
        @brief Do nothing

        @param [in] input String you want to parse.

        @return true
    */
    virtual bool parse(const char* input);
};


/*!
    @brief Parser class that accepts only characters given

    Refer to the usage below.
    @code
    Utility::CharGroupParser cgp("ab");

    cgp.parse("a");  // == true
    cgp.index();     // == 0

    cgp.parse("b");  // == true
    cgp.index();     // == 1

    cgp.parse("ax"); // == true
    cgp.index();     // == 0

    cgp.parse("c");  // == false
    cgp.index();     // == -1
    @endcode
*/
class Utility::CharGroupParser : public Utility::AbstractParser
{
private:
    const char* m_accept_chars;

public:
    /*!
        @brief Constructor

        @param [in] accept_chars Condition string.

        @note
        The class runs linear search internally,
        so it is efficient to put a char that has high priority in heading of the accept_chars.
    */
    CharGroupParser(const char* accept_chars);

    /*!
        @brief Destructor
    */
    virtual ~CharGroupParser();

    /*!
        @brief Parse input string

        In the implementation, parses the heading of the input.
        (i.e. Validate the input as <b>^[accept_chars]</b>.)

        If you want <b>^[accept_chars]+$</b> to be validated,
        please edit the method yourself.

        @param [in] input String you want to parse.

        @return Result
    */
    virtual bool parse(const char* input);
};


/*!
    @brief Parser class that accepts only strings given

    Refer to the usage below.
    @code
    const char* ACCEPT_STRS[] = {
        "AA",
        "BB",
        "CC"
    };
    const unsigned char ACCEPT_STRS_LENGTH = sizeof(ACCEPT_STRS) / sizeof(ACCEPT_STRS[0]);

    Utility::StringGroupParser sgp(ACCEPT_STRS, ACCEPT_STRS_LENGTH);

    sgp.parse("Aa"); // == true
    sgp.index();     // == 0

    sgp.parse("AA"); // == true
    sgp.index();     // == 0

    sgp.parse("CX"); // == false
    sgp.index();     // == -1
    @endcode

    @attention
    No case sensitivity on parsing.
*/
class Utility::StringGroupParser : public Utility::AbstractParser
{
private:
    const char** m_accept_strs;
    const uint8_t m_size;

public:
    /*!
        @brief Constructor

        @param [in] accept_strs Pointer of condition array sorted with dictionary order.
        @param [in] size        Size of the condition array.

        @attention
        Should give the class array sorted with dictionary order,
        because it runs binary search internally.
    */
    StringGroupParser(const char* accept_strs[], uint8_t size);

    /*!
        @brief Destructor
    */
    virtual ~StringGroupParser();

    /*!
        @brief Parse input string

        @param [in] input String you want to parse.

        @return Result
    */
    virtual bool parse(const char* input);
};


/*!
    @brief Parser class that accepts only hex string
*/
class Utility::HexStringParser : public Utility::AbstractParser
{
public:
    /*!
        @brief Constructor
    */
    HexStringParser();

    /*!
        @brief Destructor
    */
    virtual ~HexStringParser();

    /*!
        @brief Parse input string

        @param [in] input String you want to parse.

        @return Result
    */
    virtual bool parse(const char* input);
};

#endif // UTILITY_PARSER_H
