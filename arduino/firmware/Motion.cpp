/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include "Arduino.h"

#include "ExternalEEPROM.h"
#include "Motion.h"

#if DEBUG
	#include "System.h"
	#include "Profiler.h"
#endif


namespace
{
	using namespace PLEN2;
	using namespace PLEN2::Motion;


	template<const int N>
	struct IF
	{
		enum { VALUE = 1 };
	};

	template<>
	struct IF<0>
	{
		enum { VALUE = 0 };
	};

	template<typename T>
	struct SIZE_SUP
	{
		enum { VALUE = sizeof(T) % 30 /* ExternalEEPROM::SLOT_SIZE */ };
	};

	template<typename T>
	struct SLOT_COUNT
	{
		enum {
			VALUE = sizeof(T) / 30 /* ExternalEEPROM::SLOT_SIZE */
			      + IF<SIZE_SUP<T>::VALUE>::VALUE
		};
	};

	enum {
		SLOT_COUNT_HEADER = SLOT_COUNT<Header>::VALUE,
		SLOT_COUNT_FRAME  = SLOT_COUNT<Frame >::VALUE,
		SLOT_COUNT_MOTION = SLOT_COUNT_HEADER + SLOT_COUNT_FRAME * Header::FRAMELENGTH_MAX
	};
}


namespace PLEN2
{
namespace Motion
{

void Header::init()
{
	slot              = 0;
	name[0]           = '\0';
	name[NAME_LENGTH] = '\0';
	frame_length      = FRAMELENGTH_MIN;
	use_extra         = 0;
	use_jump          = 0;
	use_loop          = 0;
	loop_begin        = 0;
	loop_end          = 0;
	loop_count        = 255;
	stop_flags[0]     = 0xFF;
	stop_flags[1]     = 0xFF;
	stop_flags[2]     = 0xFF;
}


bool Header::set()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Header::set()"));
	#endif

	if (slot >= SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad instance : this->slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return false;
	}

	if (   (frame_length < FRAMELENGTH_MIN)
		|| (frame_length > FRAMELENGTH_MAX)
	)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad instance : this->frame_length = "));
			System::debugSerial().println(static_cast<int>(frame_length));
		#endif

		return false;
	}


	const char* filler = reinterpret_cast<const char*>(this);

	for (int count = 0; count < SLOT_COUNT_HEADER; count++)
	{
		int ret = ExternalEEPROM::writeSlot(
			(
				static_cast<int>(slot) * SLOT_COUNT_MOTION + count
			),
			(
				filler + count * ExternalEEPROM::SLOT_SIZE()
			),
			(
				(count == (SLOT_COUNT_HEADER - 1))?
					(
						(SIZE_SUP<Header>::VALUE)?
							SIZE_SUP<Header>::VALUE : ExternalEEPROM::SLOT_SIZE()
					)
					: ExternalEEPROM::SLOT_SIZE()
			)
		);

		if (ret != 0)
		{
			#if DEBUG
				System::debugSerial().print(F(">>> failed : ret["));
				System::debugSerial().print(count);
				System::debugSerial().print(F("] = "));
				System::debugSerial().println(ret);
			#endif

			return false;
		}
	}

	return true;
}


bool Header::get()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Header::get()"));
	#endif

	if (slot >= SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad instance : this->slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return false;
	}


	char* filler = reinterpret_cast<char*>(this);

	for (int count = 0; count < SLOT_COUNT_HEADER; count++)
	{
		int ret = ExternalEEPROM::readSlot(
			(
				static_cast<int>(slot) * SLOT_COUNT_MOTION + count
			),
			(
				filler + count * ExternalEEPROM::SLOT_SIZE()
			),
			(
				(count == (SLOT_COUNT_HEADER - 1))?
					(
						(SIZE_SUP<Header>::VALUE)?
							SIZE_SUP<Header>::VALUE : ExternalEEPROM::SLOT_SIZE()
					)
					: ExternalEEPROM::SLOT_SIZE()
			)
		);

		if (ret == -1)
		{
			#if DEBUG
				System::debugSerial().print(F(">>> failed : ret["));
				System::debugSerial().print(count);
				System::debugSerial().print(F("] = "));
				System::debugSerial().println(ret);
			#endif

			return false;
		}
	}

	return true;
}


bool Frame::set(unsigned char slot)
{
	#if DEBUG
		volatile Utility::Profiler p(F("Frame::set()"));
	#endif
	
	if (slot >= SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argment : slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return false;
	}

	if (index >= FRAME_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad instance : this->index = "));
			System::debugSerial().println(static_cast<int>(index));
		#endif

		return false;
	}


	const char* filler = reinterpret_cast<const char*>(this);

	for (int count = 0; count < SLOT_COUNT_FRAME; count++)
	{
		int ret = ExternalEEPROM::writeSlot(
			(
				  static_cast<int>(slot) * SLOT_COUNT_MOTION
				+ SLOT_COUNT_HEADER
				+ index * SLOT_COUNT_FRAME
				+ count
			),
			(
				filler + ExternalEEPROM::SLOT_SIZE() * count
			),
			(
				(count == (SLOT_COUNT_FRAME - 1))?
					(
						(SIZE_SUP<Frame>::VALUE)?
							SIZE_SUP<Frame>::VALUE : ExternalEEPROM::SLOT_SIZE()
					)
					: ExternalEEPROM::SLOT_SIZE()
			)
		);

		if (ret != 0)
		{
			#if DEBUG
				System::debugSerial().print(F(">>> failed : ret["));
				System::debugSerial().print(count);
				System::debugSerial().print(F("] = "));
				System::debugSerial().println(ret);
			#endif

			return false;
		}
	}

	return true;
}


bool Frame::get(unsigned char slot)
{
	#if DEBUG
		volatile Utility::Profiler p(F("Frame::get()"));
	#endif
	
	if (slot >= SLOT_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad argment : slot = "));
			System::debugSerial().println(static_cast<int>(slot));
		#endif

		return false;
	}

	if (index >= Frame::FRAME_END)
	{
		#if DEBUG
			System::debugSerial().print(F(">>> bad instance : this->index = "));
			System::debugSerial().println(static_cast<int>(index));
		#endif

		return false;
	}


	char* filler = reinterpret_cast<char*>(this);

	for (int count = 0; count < SLOT_COUNT_FRAME; count++)
	{
		int ret = ExternalEEPROM::readSlot(
			(
				  static_cast<int>(slot) * SLOT_COUNT_MOTION
				+ SLOT_COUNT_HEADER
				+ index * SLOT_COUNT_FRAME
				+ count
			),
			(
				filler + ExternalEEPROM::SLOT_SIZE() * count
			),
			(
				(count == (SLOT_COUNT_FRAME - 1))?
					(
						(SIZE_SUP<Frame>::VALUE)?
							SIZE_SUP<Frame>::VALUE : ExternalEEPROM::SLOT_SIZE()
					)
					: ExternalEEPROM::SLOT_SIZE()
			)
		);

		if (ret == -1)
		{
			#if DEBUG
				System::debugSerial().print(F(">>> failed : ret["));
				System::debugSerial().print(count);
				System::debugSerial().print(F("] = "));
				System::debugSerial().println(ret);
			#endif

			return false;
		}
	}

	return true;
}

} // end of namespace "Motion".
} // end of namespace "PLEN2".