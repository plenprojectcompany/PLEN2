/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

// Arduinoライブラリ
#include "Arduino.h"

// 独自ライブラリ
#include "System.h"
#include "ExternalEEPROM.h"
#include "JointController.h"
#include "MotionController.h"


// マクロの定義
#define _DEBUG      false
#define _DEBUG_HARD false


// ファイル内グローバルインスタンスの定義
namespace {
	PLEN2::System         system;
	PLEN2::ExternalEEPROM exteeprom;


	inline static const int PRECISION() { return 16; }
	
	inline long getFixedPoint(int value)
	{
		return ((long)value << PRECISION());
	}

	inline int getUnfixedPoint(long value)
	{
		return (value >> PRECISION());
	}

	template<typename T>
	int SIZE_SUP()
	{
		return (sizeof(T) % PLEN2::ExternalEEPROM::SLOT_SIZE());
	}

	template<typename T>
	int SLOTNUM()
	{
		const int slot_count = sizeof(T) / PLEN2::ExternalEEPROM::SLOT_SIZE();

		int slot_num = 0;
		slot_num += slot_count;
		slot_num += (SIZE_SUP<T>()? 1 : 0);

		return slot_num;
	}

	int SLOTNUM_MOTION_FULL()
	{
		int slot_num = 0;
		slot_num += SLOTNUM<PLEN2::MotionController::Header>();
		slot_num += SLOTNUM<PLEN2::MotionController::Frame>() * PLEN2::MotionController::Header::FRAMELENGTH_MAX();

		return slot_num;
	}
}


PLEN2::MotionController::MotionController(JointController& joint_ctrl)
{
	m_joint_ctrl_ptr = &joint_ctrl;

	m_playing = false;
	m_frame_ptr_now  = m_buffer;
	m_frame_ptr_next = m_buffer + 1;

	for (int joint_id = 0; joint_id < JointController::SUM(); joint_id++)
	{
		m_frame_ptr_now->joint_angle[joint_id] = 0;
	}
}


bool PLEN2::MotionController::setHeader(const Header* header_ptr)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::setHeader()"));
	#endif

	if (header_ptr->slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : header_ptr->slot = "));
			system.outputSerial().println((int)header_ptr->slot);
		#endif

		return false;
	}

	if (   (header_ptr->frame_length > Header::FRAMELENGTH_MAX())
		|| (header_ptr->frame_length < Header::FRAMELENGTH_MIN()) )
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : header_ptr->frame_length = "));
			system.outputSerial().println((int)header_ptr->frame_length);
		#endif

		return false;
	}

	int write_count = SLOTNUM<Header>();
	int write_size_sup = (SIZE_SUP<Header>()? SIZE_SUP<Header>() : ExternalEEPROM::SLOT_SIZE());

	const char* filler = (const char*)header_ptr;

	for (int count = 0; count < (write_count - 1); count++)
	{
		int ret = exteeprom.writeSlot(
			(int)header_ptr->slot * SLOTNUM_MOTION_FULL() + count,
			filler + ExternalEEPROM::SLOT_SIZE() * count,
			ExternalEEPROM::SLOT_SIZE()
		);

		if (ret != 0)
		{
			#if _DEBUG
				system.outputSerial().print(F(">>> failed : ret["));
				system.outputSerial().print(count);
				system.outputSerial().print(F("] = "));
				system.outputSerial().println(ret);
			#endif

			return false;
		}
	}

	int ret = exteeprom.writeSlot(
		(int)header_ptr->slot * SLOTNUM_MOTION_FULL() + (write_count - 1),
		filler + ExternalEEPROM::SLOT_SIZE() * (write_count - 1),
		write_size_sup
	);

	if (ret != 0)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> failed : ret["));
			system.outputSerial().print(write_count - 1);
			system.outputSerial().print(F("] = "));
			system.outputSerial().println(ret);
		#endif

		return false;
	}

	return true;
}


bool PLEN2::MotionController::getHeader(Header* header_ptr)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::getHeader()"));
	#endif
	
	if (header_ptr->slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : header_ptr->slot = "));
			system.outputSerial().println((int)header_ptr->slot);
		#endif

		return false;
	}

	int read_count = SLOTNUM<Header>();
	int read_size_sup = (SIZE_SUP<Header>()? SIZE_SUP<Header>() : ExternalEEPROM::SLOT_SIZE());

	char* filler = (char*)header_ptr;

	for (int count = 0; count < (read_count - 1); count++)
	{
		int ret = exteeprom.readSlot(
			(int)header_ptr->slot * SLOTNUM_MOTION_FULL() + count,
			filler + ExternalEEPROM::SLOT_SIZE() * count,
			ExternalEEPROM::SLOT_SIZE()
		);

		if (ret != ExternalEEPROM::SLOT_SIZE())
		{
			#if _DEBUG
				system.outputSerial().print(F(">>> failed : ret["));
				system.outputSerial().print(count);
				system.outputSerial().print(F("] = "));
				system.outputSerial().println(ret);
			#endif

			return false;
		}
	}

	int ret = exteeprom.readSlot(
		(int)header_ptr->slot * SLOTNUM_MOTION_FULL() + (read_count - 1),
		filler + ExternalEEPROM::SLOT_SIZE() * (read_count - 1),
		read_size_sup
	);

	if (ret != read_size_sup)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> failed : ret["));
			system.outputSerial().print(read_count - 1);
			system.outputSerial().print(F("] = "));
			system.outputSerial().println(ret);
		#endif

		return false;
	}

	return true;
}


bool PLEN2::MotionController::setFrame(unsigned char slot, const Frame* frame_ptr)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::setFrame()"));
	#endif
	
	if (slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return false;
	}

	if (frame_ptr->index >= Frame::FRAME_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : frame_ptr->index = "));
			system.outputSerial().println((int)frame_ptr->index);
		#endif

		return false;
	}

	int frame_id = frame_ptr->index;
	int write_count = SLOTNUM<Frame>();
	int write_size_sup = (SIZE_SUP<Frame>()? SIZE_SUP<Frame>() : ExternalEEPROM::SLOT_SIZE());

	const char* filler = (const char*)frame_ptr;

	for (int count = 0; count < (write_count - 1); count++)
	{
		int ret = exteeprom.writeSlot(
			(int)slot * SLOTNUM_MOTION_FULL() + SLOTNUM<Header>() + frame_id * SLOTNUM<Frame>() + count,
			filler + ExternalEEPROM::SLOT_SIZE() * count,
			ExternalEEPROM::SLOT_SIZE()
		);

		if (ret != 0)
		{
			#if _DEBUG
				system.outputSerial().print(F(">>> failed : ret["));
				system.outputSerial().print(count);
				system.outputSerial().print(F("] = "));
				system.outputSerial().println(ret);
			#endif

			return false;
		}
	}

	int ret = exteeprom.writeSlot(
		(int)slot * SLOTNUM_MOTION_FULL() + SLOTNUM<Header>() + frame_id * SLOTNUM<Frame>() + (write_count - 1),
		filler + ExternalEEPROM::SLOT_SIZE() * (write_count - 1),
		write_size_sup
	);

	if (ret != 0)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> failed : ret["));
			system.outputSerial().print(write_count - 1);
			system.outputSerial().print(F("] = "));
			system.outputSerial().println(ret);
		#endif

		return false;
	}

	return true;
}


bool PLEN2::MotionController::getFrame(unsigned char slot, Frame* frame_ptr)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::getFrame()"));
	#endif
	
	if (slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return false;
	}

	if (frame_ptr->index >= Frame::FRAME_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : frame_ptr->index = "));
			system.outputSerial().println((int)frame_ptr->index);
		#endif

		return false;
	}

	int frame_id = frame_ptr->index;
	int read_count = SLOTNUM<Frame>();
	int read_size_sup = (SIZE_SUP<Frame>()? SIZE_SUP<Frame>() : ExternalEEPROM::SLOT_SIZE());

	char* filler = (char*)frame_ptr;

	for (int count = 0; count < (read_count - 1); count++)
	{
		int ret = exteeprom.readSlot(
			(int)slot * SLOTNUM_MOTION_FULL() + SLOTNUM<Header>() + frame_id * SLOTNUM<Frame>() + count,
			filler + ExternalEEPROM::SLOT_SIZE() * count,
			ExternalEEPROM::SLOT_SIZE()
		);

		if (ret != ExternalEEPROM::SLOT_SIZE())
		{
			#if _DEBUG
				system.outputSerial().print(F(">>> failed : ret["));
				system.outputSerial().print(count);
				system.outputSerial().print(F("] = "));
				system.outputSerial().println(ret);
			#endif

			return false;
		}
	}

	int ret = exteeprom.readSlot(
		(int)slot * SLOTNUM_MOTION_FULL() + SLOTNUM<Header>() + frame_id * SLOTNUM<Frame>() + (read_count - 1),
		filler + ExternalEEPROM::SLOT_SIZE() * (read_count - 1),
		read_size_sup
	);

	if (ret != read_size_sup)
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> failed : ret["));
			system.outputSerial().print(read_count - 1);
			system.outputSerial().print(F("] = "));
			system.outputSerial().println(ret);
		#endif

		return false;
	}

	return true;
}


bool PLEN2::MotionController::playing()
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("=== running in function : MotionController::playing()"));
	#endif

	return m_playing;
}


bool PLEN2::MotionController::frameUpdatable()
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("=== running in function : MotionController::frameUpdatable()"));
	#endif

	return m_joint_ctrl_ptr->m_1cycle_finished;
}


bool PLEN2::MotionController::frameUpdateFinished()
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("=== running in function : MotionController::frameUpdateFinished()"));
	#endif

	return (m_transition_count == 0);
}


bool PLEN2::MotionController::nextFrameLoadable()
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("=== running in function : MotionController::nextFrameLoadable()"));
	#endif

	if (   (m_header.use_loop == 1)
		|| (m_header.use_jump == 1) )
	{
		return true;
	}

	return ((m_frame_ptr_next->index + 1) < m_header.frame_length);
}


void PLEN2::MotionController::play(unsigned char slot)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::play()"));
	#endif

	if (playing())
	{
		#if _DEBUG
			system.outputSerial().println(F(">>> error! : A motion has been playing."));
		#endif

		return;
	}

	if (slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return;
	}

	m_header.slot = slot;
	getHeader(&m_header);

	m_frame_ptr_next->index = 0;
	getFrame(m_header.slot, m_frame_ptr_next);

	m_transition_count = m_frame_ptr_next->transition_time_ms / Frame::UPDATE_INTERVAL_MS();

	for (int joint_id = 0; joint_id < JointController::SUM(); joint_id++)
	{
		m_now_fixed_points[joint_id] = getFixedPoint(m_frame_ptr_now->joint_angle[joint_id]);
		
		m_diff_fixed_points[joint_id] =  getFixedPoint(m_frame_ptr_next->joint_angle[joint_id]) - m_now_fixed_points[joint_id];
		m_diff_fixed_points[joint_id] /= m_transition_count;
	}

	m_playing = true;
}


void PLEN2::MotionController::stopping()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::stopping()"));
	#endif

	m_header.use_loop = 0;
	m_header.use_jump = 0;
}


void PLEN2::MotionController::stop()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::stop()"));
	#endif

	m_playing = false;
	frameBuffering(); // @attension nowフレームを正しく設定するために必須！
}


void PLEN2::MotionController::frameUpdate()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::frameUpdate()"));
	#endif	

	m_transition_count--;

	for (int joint_id = 0; joint_id < JointController::SUM(); joint_id++)
	{
		m_now_fixed_points[joint_id] += m_diff_fixed_points[joint_id];
		m_joint_ctrl_ptr->setAngleDiff(joint_id, getUnfixedPoint(m_now_fixed_points[joint_id]));
	}

	m_joint_ctrl_ptr->m_1cycle_finished = false;
}


void PLEN2::MotionController::frameBuffering()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::frameBuffering()"));
	#endif

	Frame* frame_ptr_temp = m_frame_ptr_now;
	m_frame_ptr_now  = m_frame_ptr_next;
	m_frame_ptr_next = frame_ptr_temp;
}


void PLEN2::MotionController::loadNextFrame()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::loadNextFrame()"));
	#endif

	frameBuffering();
	const unsigned char& index_now = m_frame_ptr_now->index;

	#if _DEBUG
		system.outputSerial().print(F(">>> index_now : "));
		system.outputSerial().println((int)index_now);

		system.outputSerial().print(F(">>> m_header.frame_length : "));
		system.outputSerial().println((int)m_header.frame_length);
	#endif

	/*!
		@note
		ビルトイン関数の処理の優先順位は"loop" > "jump"です。
	*/
	if (m_header.use_loop == 1)
	{
		if (index_now == m_header.loop_end)
		{
			m_frame_ptr_next->index = m_header.loop_begin;
			getFrame(m_header.slot, m_frame_ptr_next);

			if (m_header.loop_count != 255)
			{
				m_header.loop_count--;
			}

			if (m_header.loop_count == 0)
			{
				m_header.use_loop = 0;
			}
		}
		else
		{
			m_frame_ptr_next->index = index_now + 1;
			getFrame(m_header.slot, m_frame_ptr_next);
		}

		if (m_header.use_loop != 0)
		{
			goto update_process;
		}
	}

	if (m_header.use_jump == 1)
	{
		if (index_now == (m_header.frame_length - 1))
		{
			m_header.slot = m_header.jump_slot;
			getHeader(&m_header);

			m_frame_ptr_next->index = 0;
			getFrame(m_header.slot, m_frame_ptr_next);
		}
		else
		{
			m_frame_ptr_next->index = index_now + 1;
			getFrame(m_header.slot, m_frame_ptr_next);
		}

		goto update_process;
	}

	if (index_now == (m_header.frame_length - 1))
	{
		m_playing = false; // sanity check.

		return;
	}

	m_frame_ptr_next->index = index_now + 1;
	getFrame(m_header.slot, m_frame_ptr_next);


update_process:
	m_transition_count = m_frame_ptr_next->transition_time_ms / Frame::UPDATE_INTERVAL_MS();

	for (int joint_id = 0; joint_id < JointController::SUM(); joint_id++)
	{
		m_now_fixed_points[joint_id] = getFixedPoint(m_frame_ptr_now->joint_angle[joint_id]);
		
		m_diff_fixed_points[joint_id] = getFixedPoint(m_frame_ptr_next->joint_angle[joint_id]) - m_now_fixed_points[joint_id];
		m_diff_fixed_points[joint_id] /= m_transition_count;
	}
}


void PLEN2::MotionController::dump(unsigned char slot)
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : MotionController::dump()"));
	#endif

	if (slot >= Header::SLOT_END())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> bad argment : slot = "));
			system.outputSerial().println((int)slot);
		#endif

		return;
	}

	Header header;
	header.slot = slot;
	getHeader(&header);

	system.outputSerial().println(F("{"));

		system.outputSerial().print(F("\t\"slot\": "));
		system.outputSerial().print((int)header.slot);
		system.outputSerial().println(F(","));

		system.outputSerial().print(F("\t\"name\": \""));
		header.name[Header::NAME_LENGTH()] = '\0'; // @note sanity check.
		system.outputSerial().print(header.name);
		system.outputSerial().println(F("\","));

		system.outputSerial().println(F("\t\"codes\": ["));

			if (header.use_loop == 1)
			{
			system.outputSerial().println(F("\t\t{"));

				system.outputSerial().println(F("\t\t\t\"func\": \"loop\","));
				system.outputSerial().print(F("\t\t\t\"args\": ["));

					system.outputSerial().print((int)header.loop_begin);
					system.outputSerial().print(F(", "));
					system.outputSerial().print((int)header.loop_end);
					system.outputSerial().print(F(", "));
					system.outputSerial().print((int)header.loop_count);

				system.outputSerial().println(F("]"));

			system.outputSerial().println(F("\t\t}"));
			}

			if (header.use_jump == 1)
			{
			system.outputSerial().println(F("\t\t{"));

				system.outputSerial().println(F("\t\t\t\"func\": \"jump\","));
				system.outputSerial().print(F("\t\t\t\"args\": ["));

					system.outputSerial().print((int)header.jump_slot);

				system.outputSerial().println(F("]"));

			system.outputSerial().println(F("\t\t}"));
			}

		system.outputSerial().println(F("\t],"));

		system.outputSerial().println(F("\t\"frames\": ["));

		for (int frame_index = 0; frame_index < header.frame_length; frame_index++)
		{
			Frame frame;
			frame.index = frame_index;
			getFrame(header.slot, &frame);

			system.outputSerial().println(F("\t\t{"));

				system.outputSerial().print(F("\t\t\t\"transition_time_ms\": "));
				system.outputSerial().print(frame.transition_time_ms);
				system.outputSerial().println(F(","));

				system.outputSerial().println(F("\t\t\t\"outputs\": ["));

				for (int device_index = 0; device_index < JointController::SUM(); device_index++)
				{
					system.outputSerial().println(F("\t\t\t\t{"));

						system.outputSerial().print(F("\t\t\t\t\t\"device\": "));
						system.outputSerial().print(device_index);
						system.outputSerial().println(F(","));

						system.outputSerial().print(F("\t\t\t\t\t\"value\": "));
						system.outputSerial().println(frame.joint_angle[device_index]);

					if ((device_index + 1) == JointController::SUM())
					{
					system.outputSerial().println(F("\t\t\t\t}"));
					}
					else
					{
					system.outputSerial().println(F("\t\t\t\t},"));
					}
				}

				system.outputSerial().println(F("\t\t\t]"));

			if ((frame_index + 1) == header.frame_length)
			{
			system.outputSerial().println(F("\t\t}"));
			}
			else
			{
			system.outputSerial().println(F("\t\t},"));
			}
		}

		system.outputSerial().println(F("\t]"));

	system.outputSerial().println(F("}"));
}