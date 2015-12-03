/*!
	@file      Protocol.h
	@brief     Analysis class of the PLEN2's protocol.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_PROTOCOL_H
#define PLEN2_PROTOCOL_H


namespace PLEN2
{
	class Protocol;
}

namespace Utility
{
	class AbstractParser;
}

/*!
	@brief Analysis class of the PLEN2's protocol

	The class only analyses the command line given,
	so should override the event handler(s) with inheriting the class yourself.

	Please see the virtual methods to get more details.
*/
class PLEN2::Protocol
{
protected:
	/*!
		@brief List of the internal states
	*/
	typedef enum
	{
		READY,               //!< Ready.
		HEADER_INCOMING = 0, //!< Will receive string that might be HEADER. (Alias of state READY.)
		COMMAND_INCOMING,    //!< Will receive string that might be COMMAND.
		ARGUMENTS_INCOMING,  //!< Will receive string that might be ARGUMENTS.
		STATE_EOE            //!< Summation of the states.
	} State;

	/*!
		@brief Buffer struct
	*/
	class Buffer
	{
	public:
		enum {
			/*!
				@brief Buffer length

				@attention
				The value is required at least more than BLE payload length (= 20 bytes),
				and it should be defined 2^N length for processing the class with high speed.
			*/
			LENGTH = 128
		};

		char          data[LENGTH]; //!< Actual buffer instance.
		unsigned char position;     //!< Current iterator's position.

		/*!
			@brief Constructor
		*/
		Buffer()
			: position(0)
		{
			for (unsigned char index = 0; index < LENGTH; index++)
			{
				data[index] = '\0';
			}
		}
	};

	Buffer m_buffer;
	State m_state;
	unsigned char m_store_length;
	bool m_installing;
	Utility::AbstractParser* m_parser[STATE_EOE];

	/*!
		@brief Abort analysis
	*/
	void m_abort();

public:
	/*!
		@brief Constructor
	*/
	Protocol();

	/*!
		@brief Destructor
	*/
	virtual ~Protocol() {}

	/*!
		@brief Read a character, and store it in the ring buffer

		@param [in] byte A character.
	*/
	void readByte(char byte);

	/*!
		@brief Accept buffered string considering internal state

		@return Result
	*/
	bool accept();

	/*!
		@brief Transit internal state
	*/
	void transitState();

	/*!
		@brief User-defined hook that runs before transitState()
	*/
	virtual void beforeHook();

	/*!
		@brief User-defined hook that runs after transitState()
	*/
	virtual void afterHook();
};

#endif // PLEN2_PROTOCOL_H