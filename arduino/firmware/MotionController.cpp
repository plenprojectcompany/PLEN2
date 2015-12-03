/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG      false
#define DEBUG_HARD false

#include <Arduino.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"

#if DEBUG || DEBUG_HARD
	#include "Profiler.h"
#endif


namespace
{
	enum { PRECISION = 16 };

	inline const long fixed_cast(const int value)
	{
		return (static_cast<long>(value) << PRECISION);
	}

	inline const int unfixed_cast(const long value)
	{
		return static_cast<int>(value >> PRECISION);
	}
}


PLEN2::MotionController::MotionController(JointController& joint_ctrl)
{
	m_joint_ctrl_ptr = &joint_ctrl;

	m_playing = false;
	m_frame_current_ptr = m_buffer;
	m_frame_next_ptr    = m_buffer + 1;

	for (char joint_id = 0; joint_id < JointController::SUM; joint_id++)
	{
		m_frame_current_ptr->joint_angle[joint_id] = 0;
	}
}


bool PLEN2::MotionController::playing()
{
	#if DEBUG_HARD
		volatile Utility::Profiler p(F("MotionController::playing()"));
	#endif

	return m_playing;
}


bool PLEN2::MotionController::frameUpdatable()
{
	#if DEBUG_HARD
		volatile Utility::Profiler p(F("MotionController::frameUpdatable()"));
	#endif

	return m_joint_ctrl_ptr->m_1cycle_finished;
}


bool PLEN2::MotionController::updatingFinished()
{
	#if DEBUG_HARD
		volatile Utility::Profiler p(F("MotionController::updatingFinished()"));
	#endif

	return (m_transition_count == 0);
}


bool PLEN2::MotionController::nextFrameLoadable()
{
	#if DEBUG_HARD
		volatile Utility::Profiler p(F("MotionController::nextFrameLoadable()"));
	#endif

	if (   (m_header.use_loop)
		|| (m_header.use_jump) )
	{
		return true;
	}

	return ((m_frame_next_ptr->index + 1) < m_header.frame_length);
}


void PLEN2::MotionController::play(unsigned char slot)
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::play()"));
	#endif

	if (playing())
	{
		#if DEBUG
			System::debugSerial().println(F(">>> error : A motion has been playing."));
		#endif

		return;
	}

	if (slot >= Motion::SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argment : slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return;
	}


	m_header.slot = slot;
	m_header.get();

	m_setupFrame(0);

	m_playing = true;
}


void PLEN2::MotionController::willStop()
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::willStop()"));
	#endif

	m_header.use_loop = 0;
	m_header.use_jump = 0;
}


void PLEN2::MotionController::stop()
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::stop()"));
	#endif

	m_playing = false;
	m_bufferingFrame(); // @attension It is necessary for a valid sequence!
}


void PLEN2::MotionController::updateFrame()
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::updateFrame()"));
	#endif

	m_transition_count--;

	for (char joint_id = 0; joint_id < JointController::SUM; joint_id++)
	{
		m_current_fixed_points[joint_id] += m_diff_fixed_points[joint_id];
		m_joint_ctrl_ptr->setAngleDiff(joint_id, unfixed_cast(m_current_fixed_points[joint_id]));
	}

	m_joint_ctrl_ptr->m_1cycle_finished = false;
}


void PLEN2::MotionController::m_setupFrame(unsigned char index)
{
	m_frame_next_ptr->index = index;
	m_frame_next_ptr->get(m_header.slot);

	m_transition_count = m_frame_next_ptr->transition_time_ms / Motion::Frame::UPDATE_INTERVAL_MS;

	for (char joint_id = 0; joint_id < JointController::SUM; joint_id++)
	{
		m_current_fixed_points[joint_id] = fixed_cast(m_frame_current_ptr->joint_angle[joint_id]);

		m_diff_fixed_points[joint_id]  = fixed_cast(m_frame_next_ptr->joint_angle[joint_id]) - m_current_fixed_points[joint_id];
		m_diff_fixed_points[joint_id] /= m_transition_count;
	}
}


void PLEN2::MotionController::m_bufferingFrame()
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::m_bufferingFrame()"));
	#endif

	Motion::Frame* frame_ptr_temp = m_frame_current_ptr;
	m_frame_current_ptr = m_frame_next_ptr;
	m_frame_next_ptr    = frame_ptr_temp;
}


void PLEN2::MotionController::loadNextFrame()
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::loadNextFrame()"));
	#endif

	m_bufferingFrame();
	const unsigned char index_now = m_frame_current_ptr->index;

	#if DEBUG
		System::debugSerial().print(F(">>> index_now : "));
		System::debugSerial().println(static_cast<int>(index_now));

		System::debugSerial().print(F(">>> m_header.frame_length : "));
		System::debugSerial().println(static_cast<int>(m_header.frame_length));
	#endif

	/*!
		@note
		The order of priority of doing built-in functions, is "loop" > "jump".
	*/
	if (m_header.use_loop)
	{
		if (index_now >= m_header.loop_end)
		{
			m_setupFrame(m_header.loop_begin);

			if (m_header.loop_count != 255)
			{
				m_header.loop_count--;
			}

			if (m_header.loop_count == 0)
			{
				m_header.use_loop = 0;
			}

			return;
		}
	}

	if (   (!m_header.use_loop)
		&& (m_header.use_jump)
		&& (index_now >= (m_header.frame_length - 1)) )
	{
		m_header.slot = m_header.jump_slot;
		m_header.get();
		m_setupFrame(0);

		return;
	}

	m_setupFrame(index_now + 1);
}


void PLEN2::MotionController::dump(unsigned char slot)
{
	#if DEBUG
		volatile Utility::Profiler p(F("MotionController::dump()"));
	#endif

	if (slot >= Motion::SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argment : slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return;
	}


	Motion::Header header;
	header.slot = slot;
	header.get();

	System::outputSerial().println(F("{"));

	System::outputSerial().print(F("\t\"slot\": "));
	System::outputSerial().print(static_cast<int>(header.slot));
	System::outputSerial().println(F(","));

	header.name[Motion::Header::NAME_LENGTH - 1] = '\0'; // sanity check.
	System::outputSerial().print(F("\t\"name\": \""));
	System::outputSerial().print(header.name);
	System::outputSerial().println(F("\","));

	System::outputSerial().println(F("\t\"codes\": ["));

	if (header.use_loop)
	{
		System::outputSerial().println(F("\t\t{"));

		System::outputSerial().println(F("\t\t\t\"func\": \"loop\","));

		System::outputSerial().print(F("\t\t\t\"args\": ["));

		System::outputSerial().print(static_cast<int>(header.loop_begin));
		System::outputSerial().print(F(", "));

		System::outputSerial().print(static_cast<int>(header.loop_end));
		System::outputSerial().print(F(", "));

		System::outputSerial().print(static_cast<int>(header.loop_count));

		System::outputSerial().println(F("]"));

		System::outputSerial().print(F("\t\t}"));

		if (header.use_jump)
		{
			System::outputSerial().print(F(","));
		}

		System::outputSerial().println();
	}

	if (header.use_jump == 1)
	{
		System::outputSerial().println(F("\t\t{"));

		System::outputSerial().println(F("\t\t\t\"func\": \"jump\","));

		System::outputSerial().print(F("\t\t\t\"args\": ["));

		System::outputSerial().print(static_cast<int>(header.jump_slot));

		System::outputSerial().println(F("]"));

		System::outputSerial().println(F("\t\t}"));
	}

	System::outputSerial().println(F("\t],"));

	System::outputSerial().println(F("\t\"frames\": ["));

	Motion::Frame frame;

	for (char frame_index = 0; frame_index < header.frame_length; frame_index++)
	{
		frame.index = frame_index;
		frame.get(header.slot);

		System::outputSerial().println(F("\t\t{"));

		System::outputSerial().print(F("\t\t\t\"transition_time_ms\": "));
		System::outputSerial().print(frame.transition_time_ms);
		System::outputSerial().println(F(","));

		System::outputSerial().println(F("\t\t\t\"outputs\": ["));

		for (int device_index = 0; device_index < JointController::SUM; device_index++)
		{
			System::outputSerial().println(F("\t\t\t\t{"));

			System::outputSerial().print(F("\t\t\t\t\t\"device\": "));
			System::outputSerial().print(device_index);
			System::outputSerial().println(F(","));

			System::outputSerial().print(F("\t\t\t\t\t\"value\": "));
			System::outputSerial().println(frame.joint_angle[device_index]);

			if ((device_index + 1) == JointController::SUM)
			{
				System::outputSerial().println(F("\t\t\t\t}"));
			}
			else
			{
				System::outputSerial().println(F("\t\t\t\t},"));
			}
		}

		System::outputSerial().println(F("\t\t\t]"));

		System::outputSerial().print(F("\t\t}"));

		if ((frame_index + 1) == header.frame_length)
		{
			System::outputSerial().println();
		}
		else
		{
			System::outputSerial().println(F(","));
		}
	}

	System::outputSerial().println(F("\t]"));

	System::outputSerial().println(F("}"));
}