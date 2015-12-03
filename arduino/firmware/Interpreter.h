/*!
	@file      Interpreter.h
	@brief     Management class of interpreter.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_INTERPRETER_H
#define PLEN2_INTERPRETER_H

namespace PLEN2
{
	class Interpreter;
	class MotionController;
}

/*!
	@brief Management class of interpreter

	@attention
	The class gives private members (of a motion controller) an effect,
	so you might get an unexpected result when running the methods, with a complex sequence structure.
*/
class PLEN2::Interpreter
{
public:
	/*!
		@brief Code struct
	*/
	class Code
	{
	public:
		unsigned char slot;       //!< Slot number of a motion.
		unsigned char loop_count; //!< Loop count. (Using 255 as infinity.)
	};

	enum {
		/*!
			@brief Size of code queue

			@attention
			It should be defined 2^N length for processing the class with high speed.
		*/
		QUEUE_SIZE = 32
	};


	/*!
		@brief Constructor

		@param [in, out] motion_ctrl Instance of a motion controller.
	*/
	Interpreter(MotionController& motion_ctrl);

	/*!
		@brief Reserve to run a code

		@param [in] code Instance of a code.

		@return Result
		@retval true  Succeeded to push a code to the queue.
		@retval false The queue is overflowed.
	*/
	bool pushCode(const Code& code);

	/*!
		@brief Run a code in heading of the queue.

		@return Result
		@retval true  Succeeded to pop a code from the queue. (However, running a code might not be successful.)
		@retval false The queue is empty.

		@attention
		The order of internal processing is "play a motion.", "Rewrite the header.",
		so you might get an unexpected result when running the method along with playing a motion.
	*/
	bool popCode();

	/*!
		@brief Decide there are codes which are reserved to run

		@return Result
	*/
	bool ready();

	/*!
		@brief Reset the interpreter
	*/
	void reset();


private:
	Code m_code_queue[QUEUE_SIZE];
	unsigned char m_queue_begin;
	unsigned char m_queue_end;
	MotionController* m_motion_ctrl_ptr;
};

#endif // PLEN2_INTERPRETER_H