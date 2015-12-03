/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

/*!
	@note
	If you want to apply natural moving on PLEN, set the macro to "true".
	(The process is using a lot of calculation space, so the firmware becomes hard to input user action.)
*/
#define ENSOUL_PLEN2 false


#include <string.h>

#include <EEPROM.h>
#include <Wire.h>

#include "ExternalEEPROM.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Interpreter.h"
#include "Pin.h"
#include "Parser.h"
#include "Protocol.h"
#include "System.h"

#if ENSOUL_PLEN2
	#include "AccelerationGyroSensor.h"
	#include "Soul.h"
#endif

#if DEBUG
	#include "Profiler.h"
#endif


namespace
{
	using namespace PLEN2;

	/*!
		Core instances
	*/
	JointController  joint_ctrl;
	MotionController motion_ctrl(joint_ctrl);
	Interpreter      interpreter(motion_ctrl);

	#if ENSOUL_PLEN2
		AccelerationGyroSensor sensor;
		Soul                   soul(sensor, motion_ctrl);
	#endif

	/*!
		The application instance
	*/
	class Application : public Protocol
	{
	private:
		static void (Application::*CONTROLLER_EVENT_HANDLER[])();
		static void (Application::*INTERPRETER_EVENT_HANDLER[])();
		static void (Application::*SETTER_EVENT_HANDLER[])();
		static void (Application::*GETTER_EVENT_HANDLER[])();

		static void (Application::**EVENT_HANDLER[])();

		Motion::Header    m_header_tmp;
		Motion::Frame     m_frame_tmp;
		Interpreter::Code m_code_tmp;

		void applyDiff()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::applyDiff()"));

				System::debugSerial().print(F(">>> joint_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> angle_diff : "));
				System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			joint_ctrl.setAngleDiff(
				Utility::hexbytes2uint(m_buffer.data, 2),
				Utility::hexbytes2int(m_buffer.data + 2, 3)
			);
		}

		void apply()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::apply()"));

				System::debugSerial().print(F(">>> joint_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> angle : "));
				System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			joint_ctrl.setAngle(
				Utility::hexbytes2uint(m_buffer.data, 2),
				Utility::hexbytes2int(m_buffer.data + 2, 3)
			);
		}

		void homePosition()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::homePosition()"));
			#endif

			joint_ctrl.loadSettings();
		}

		void playMotion()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::playMotion()"));

				System::debugSerial().print(F(">>> slot : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));
			#endif

			motion_ctrl.play(
				Utility::hexbytes2uint(m_buffer.data, 2)
			);
		}

		void stopMotion()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::stopMotion()"));
			#endif

			motion_ctrl.willStop();
		}

		void popCode()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::popCode()"));
			#endif

			interpreter.popCode();
		}

		void pushCode()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::pushCode()"));

				System::debugSerial().print(F(">>> slot : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> loop_count : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 2, 2));
			#endif

			m_code_tmp.slot       = Utility::hexbytes2uint(m_buffer.data, 2);
			m_code_tmp.loop_count = Utility::hexbytes2uint(m_buffer.data + 2, 2) - 1;

			interpreter.pushCode(m_code_tmp);
		}

		void resetInterpreter()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::resetInterpreter()"));
			#endif

			interpreter.reset();
		}

		void setHome()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::setHome()"));

				System::debugSerial().print(F(">>> joint_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> angle : "));
				System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			joint_ctrl.setHomeAngle(
				Utility::hexbytes2uint(m_buffer.data, 2),
				Utility::hexbytes2int(m_buffer.data + 2, 3)
			);
		}

		void setJointSettings()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::setJointSettings()"));
			#endif

			joint_ctrl.resetSettings();
		}

		void setMax()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::setMax()"));

				System::debugSerial().print(F(">>> joint_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> angle : "));
				System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			joint_ctrl.setMaxAngle(
				Utility::hexbytes2uint(m_buffer.data, 2),
				Utility::hexbytes2int(m_buffer.data + 2, 3)
			);
		}

		void setMotionFrame()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::setMotionFrame()"));

				System::debugSerial().print(F(">>> slot : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> frame_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 2, 2));

				System::debugSerial().print(F(">>> transition_time_ms : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 4, 4));

				for (int device_id = 0; device_id < JointController::SUM; device_id++)
				{
					System::debugSerial().print(F(">>> output["));
					System::debugSerial().print(device_id);
					System::debugSerial().print(F("] : "));
					System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 8 + device_id * 4, 4));
				}
			#endif

			m_frame_tmp.transition_time_ms = Utility::hexbytes2uint(m_buffer.data + 4, 4);

			for (char device_id = 0; device_id < JointController::SUM; device_id++)
			{
				m_frame_tmp.joint_angle[device_id] = Utility::hexbytes2int(m_buffer.data + 8 + device_id * 4, 4);
			}

			if (m_installing)
			{
				if (m_frame_tmp.index < m_header_tmp.frame_length)
				{
					m_frame_tmp.set(m_header_tmp.slot);
					m_frame_tmp.index++;
				}

				if (m_frame_tmp.index == m_header_tmp.frame_length)
				{
					m_installing = false;
				}
				else
				{
					readByte('>');
					accept();
					transitState();

					readByte('m');
					readByte('f');
					accept();
					transitState();

					readByte('0'); // dummy
					readByte('0'); // dummy
					readByte('0'); // dummy
					readByte('0'); // dummy
				}
			}
			else
			{
				m_frame_tmp.index = Utility::hexbytes2uint(m_buffer.data + 2, 2);
				m_frame_tmp.set(Utility::hexbytes2uint(m_buffer.data, 2));
			}
		}

		void setMotionHeader()
		{
			strncpy(m_header_tmp.name, m_buffer.data + 2, 20);
			m_header_tmp.name[20] = '\0';
			m_buffer.data[2]      = '\0';

			if (   !(m_parser[ARGUMENTS_INCOMING]->parse(m_buffer.data))
				|| !(m_parser[ARGUMENTS_INCOMING]->parse(m_buffer.data + 22)) )
			{
				return;
			}

			#if DEBUG
				volatile Utility::Profiler p(F("Application::setMotionHeader()"));

				System::debugSerial().print(F(">>> slot : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> name : "));
				System::debugSerial().println(m_header_tmp.name);

				System::debugSerial().print(F(">>> func : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 22, 2));

				System::debugSerial().print(F(">>> arg0 : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 24, 2));

				System::debugSerial().print(F(">>> arg1 : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 26, 2));

				System::debugSerial().print(F(">>> frame_length : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data + 28, 2));
			#endif

			m_header_tmp.slot         = Utility::hexbytes2uint(m_buffer.data, 2);
			m_header_tmp.frame_length = Utility::hexbytes2uint(m_buffer.data + 28, 2);

			switch (Utility::hexbytes2uint(m_buffer.data + 22, 2))
			{
				case 0:
				{
					m_header_tmp.use_loop = 0;
					m_header_tmp.use_jump = 0;

					break;
				}

				case 1:
				{
					m_header_tmp.use_loop   = 1;
					m_header_tmp.use_jump   = 0;
					m_header_tmp.loop_begin = Utility::hexbytes2uint(m_buffer.data + 24, 2);
					m_header_tmp.loop_end   = Utility::hexbytes2uint(m_buffer.data + 26, 2);

					break;
				}

				case 2:
				{
					m_header_tmp.use_loop  = 0;
					m_header_tmp.use_jump  = 1;
					m_header_tmp.jump_slot = Utility::hexbytes2uint(m_buffer.data + 24, 2);

					break;
				}
			}

			m_header_tmp.set();

			if (m_installing == true)
			{
				readByte('>');
				accept();
				transitState();

				readByte('m');
				readByte('f');
				accept();
				transitState();

				readByte('0'); // dummy
				readByte('0'); // dummy
				readByte('0'); // dummy
				readByte('0'); // dummy

				m_frame_tmp.index = 0;
			}
		}

		void setMin()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::setMin()"));

				System::debugSerial().print(F(">>> joint_id : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				System::debugSerial().print(F(">>> angle : "));
				System::debugSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			joint_ctrl.setMinAngle(
				Utility::hexbytes2uint(m_buffer.data, 2),
				Utility::hexbytes2int(m_buffer.data + 2, 3)
			);
		}

		void getJointSettings()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::getJointSettings()"));
			#endif

			joint_ctrl.dump();
		}

		void getMotion()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::getMotion()"));

				System::debugSerial().print(F(">>> slot : "));
				System::debugSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));
			#endif

			motion_ctrl.dump(
				Utility::hexbytes2uint(m_buffer.data, 2)
			);
		}

		void getVersionInformation()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::getVersionInformation()"));
			#endif

			System::dump();
		}

	public:
		virtual void afterHook()
		{
			#if DEBUG
				volatile Utility::Profiler p(F("Application::afterFook()"));
			#endif

			if (m_state == HEADER_INCOMING)
			{
				unsigned char header_id = m_parser[HEADER_INCOMING ]->index();
				unsigned char cmd_id    = m_parser[COMMAND_INCOMING]->index();

				(this->*EVENT_HANDLER[header_id][cmd_id])();

				#if ENSOUL_PLEN2
					soul.userActionInputed();
				#endif
			}
		}
	};

	void (Application::*Application::CONTROLLER_EVENT_HANDLER[])() = {
		&Application::applyDiff,
		&Application::apply,
		&Application::homePosition,
		&Application::playMotion,
		&Application::stopMotion,
		&Application::playMotion,
		&Application::stopMotion
	};

	void (Application::*Application::INTERPRETER_EVENT_HANDLER[])() = {
		&Application::popCode,
		&Application::pushCode,
		&Application::resetInterpreter
	};

	void (Application::*Application::SETTER_EVENT_HANDLER[])() = {
		&Application::setHome,
		&Application::setMotionHeader, // sanity check.
		&Application::setJointSettings,
		&Application::setMax,
		&Application::setMotionFrame,
		&Application::setMotionHeader,
		&Application::setMin
	};

	void (Application::*Application::GETTER_EVENT_HANDLER[])() = {
		&Application::getJointSettings,
		&Application::getMotion,
		&Application::getVersionInformation
	};

	void (Application::**Application::EVENT_HANDLER[])() = {
		Application::CONTROLLER_EVENT_HANDLER,
		Application::INTERPRETER_EVENT_HANDLER,
		Application::SETTER_EVENT_HANDLER,
		Application::GETTER_EVENT_HANDLER
	};

	Application app;
}


/*!
	@brief Setup

	Put your setup code here, to run once:

	@attention
	Digital pin's output is an indefinite if you don't give an initialize value.
	Please ensure that setup the pins which are configurable.
*/
void setup()
{
	volatile PLEN2::System         system;
	volatile PLEN2::ExternalEEPROM exteeprom;

	randomSeed( analogRead(PLEN2::Pin::RANDOM_DEVICE_IN()) );

	joint_ctrl.loadSettings();

	#if ENSOUL_PLEN2
		/*!
			@attention
			The order of power supplied or firmware startup timing is base-board, head-board.
			If the sampling method calls from early timing, program freezes because synchronism of communication is missed.
			(Generally, it is going to success setup() inserts 3000[msec] delays.)
		*/
		delay(3000);
	#endif

	#if DEBUG
		while (!Serial);

		PLEN2::System::outputSerial().println(
			F("Hello, I am PLEN2 ! My system is up and running ver.1.3.0, Let me walk :)")
		);
	#endif
}


/*!
	@brief Main polling loop

	Put your main code here, to run repeatedly:
*/
void loop()
{
	if (motion_ctrl.playing())
	{
		if (motion_ctrl.frameUpdatable())
		{
			motion_ctrl.updateFrame();
		}

		if (motion_ctrl.updatingFinished())
		{
			if (motion_ctrl.nextFrameLoadable())
			{
				motion_ctrl.loadNextFrame();
			}
			else
			{
				motion_ctrl.stop();

				if (interpreter.ready())
				{
					interpreter.popCode();
				}
			}
		}
	}

	if (PLEN2::System::USBSerial().available())
	{
		app.readByte(PLEN2::System::USBSerial().read());

		if (app.accept())
		{
			app.transitState();

		}
	}

	if (PLEN2::System::BLESerial().available())
	{
		app.readByte(PLEN2::System::BLESerial().read());

		if (app.accept())
		{
			app.transitState();
		}
	}

	#if ENSOUL_PLEN2
		soul.log();
		soul.action();
	#endif
}
