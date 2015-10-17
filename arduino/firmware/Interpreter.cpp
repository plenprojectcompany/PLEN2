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

// 独自ライブラリ
#if _DEBUG
	#include "System.h"
#endif
#include "Interpreter.h"
#include "JointController.h"
#include "MotionController.h"


namespace
{
	#if _DEBUG
		PLEN2::System system;
	#endif

	inline unsigned char getIndex(unsigned char value)
	{
		return (value & (PLEN2::Interpreter::QUEUE_SIZE() - 1));
	}
}


PLEN2::Interpreter::Interpreter(MotionController& motion_crtl)
	: m_queue_begin(0)
	, m_queue_end(0)
	, m_motion_ctrl_ptr(&motion_crtl)
{
	// noop.
}

bool PLEN2::Interpreter::pushCode(const Code& code)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Interpreter::pushCode()"));
	#endif

	if (getIndex(m_queue_end + 1) == m_queue_begin)
	{
		#if _DEBUG
			system.outputSerial().println(F(">>> error : Queue overflow!"));
		#endif

		return false;
	}

	m_code_queue[m_queue_end] = code;
	m_queue_end = getIndex(m_queue_end + 1);

	return true;
}

bool PLEN2::Interpreter::popCode()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Interpreter::popCode()"));
	#endif

	if (!ready())
	{
		#if _DEBUG
			system.outputSerial().println(F(">>> error : This is not ready!"));
		#endif

		return false;
	}

	Code& doing = m_code_queue[m_queue_begin];
	m_queue_begin = getIndex(m_queue_begin + 1);

	m_motion_ctrl_ptr->play(doing.slot);

	if (doing.loop_count != 0)
	{
		if (m_motion_ctrl_ptr->m_header.use_loop == 0)
		{
			m_motion_ctrl_ptr->m_header.use_loop = 1;

			m_motion_ctrl_ptr->m_header.loop_begin = 0;
			m_motion_ctrl_ptr->m_header.loop_end   = m_motion_ctrl_ptr->m_header.frame_length - 1;
		}
	}
	else
	{
		m_motion_ctrl_ptr->m_header.use_loop = 0;
	}

	m_motion_ctrl_ptr->m_header.use_jump = 0;
	m_motion_ctrl_ptr->m_header.loop_count = doing.loop_count;

	return true;
}

bool PLEN2::Interpreter::ready()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Interpreter::ready()"));
	#endif

	return (m_queue_begin != m_queue_end);
}

void PLEN2::Interpreter::reset()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Interpreter::reset()"));
	#endif

	m_queue_begin = 0;
	m_queue_end   = 0;
	m_motion_ctrl_ptr->stop();
}