/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

/*!
	@note
	このマクロの値をtrueにすることで、PLENに自然な動きを適用します。
	ただし、シリアル通信の帯域を圧迫するため、ユーザからの操作を受け付けづらくなります。
*/
#define _ENSOUL_PLEN2 false


// 標準ライブラリ
#include <string.h>

// Arduinoライブラリ
#include <EEPROM.h>
#include <Wire.h>

// 独自ライブラリ
#include "Interpreter.h"
#include "JointController.h"
#include "MotionController.h"
#include "Pin.h"
#include "Purser.h"
#include "PurserCombinator.h"
#include "System.h"

#if _ENSOUL_PLEN2
	#include "AccelerationGyroSensor.h"
	#include "Soul.h"
#endif


namespace Utility
{
	/*!
		@brief 16進文字配列をunisigned intに変換するメソッド

		@param [in] bytes 文字配列バッファへのポインタ
		@param [in] size  文字配列長
	*/
	unsigned int hexbytes2uint(const char* bytes, unsigned char size)
	{
		unsigned int result = 0;

		for (unsigned char index = 0; index < size; index++)
		{
			unsigned int placeholder = bytes[index];

			if (placeholder >= 'a') placeholder -= ('a' - 10);
			if (placeholder >= 'A') placeholder -= ('A' - 10);
			if (placeholder >= '0') placeholder -= '0';

			unsigned int base = 1 << ((size - index - 1) * 4);

			result += placeholder * base;
		}

		return result;
	}

	/*!
		@brief 16進文字配列をintに変換するメソッド

		@note
		16進文字配列をビット表現に変換した後、最上位ビットを符号部と見なす変換を行います。

		@param [in] bytes 文字配列バッファへのポインタ
		@param [in] size  文字配列長
	*/
	int hexbytes2int(const char* bytes, unsigned char size)
	{
		unsigned int temp = hexbytes2uint(bytes, size);
		temp <<= (((sizeof(int) * 2) - size) * 4);

		/*!
			@note
			avr-gccは算術シフトをサポートしているため、(*)の行は必要ない。
		*/
		int result = temp;
		// bool negative = (result < 0); // (*)

		result >>= (((sizeof(int) * 2) - size) * 4);
		// if (negative) result |= (0xffff << (size * 4)); // (*)

		return result;
	}
}


namespace
{
	// コアインスタンス
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
	PLEN2::Interpreter      interpreter(motion_ctrl);
	PLEN2::System           system;

	#if _ENSOUL_PLEN2
		PLEN2::AccelerationGyroSensor sensor;
		PLEN2::Soul soul(sensor, motion_ctrl);
	#endif


	// アプリケーションインスタンス
	class Application : public PLEN2::PurserCombinator
	{
	private:
		static void (Application::*CONTROLLER_EVENT_HANDLER[])();
		static void (Application::*INTERPRETER_EVENT_HANDLER[])();
		static void (Application::*SETTER_EVENT_HANDLER[])();
		static void (Application::*GETTER_EVENT_HANDLER[])();

		static void (Application::**EVENT_HANDLER[])();

		PLEN2::MotionController::Header m_header_tmp;
		PLEN2::MotionController::Frame  m_frame_tmp;
		PLEN2::Interpreter::Code        m_code_tmp;

		void applyDiff()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::applyDiff()"));

				system.outputSerial().print(F("> joint_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> angle_diff : "));
				system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			#if !_DEBUG
				joint_ctrl.setAngleDiff(
					Utility::hexbytes2uint(m_buffer.data, 2),
					Utility::hexbytes2int(m_buffer.data + 2, 3)
				);
			#endif
		}

		void apply()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::apply()"));

				system.outputSerial().print(F("> joint_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> angle : "));
				system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			#if !_DEBUG
				joint_ctrl.setAngle(
					Utility::hexbytes2uint(m_buffer.data, 2),
					Utility::hexbytes2int(m_buffer.data + 2, 3)
				);
			#endif
		}

		void homePosition()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::homePosition()"));
			#endif

			#if !_DEBUG
				joint_ctrl.loadSettings();
			#endif
		}

		void playMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::playMotion()"));

				system.outputSerial().print(F("> slot : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));
			#endif

			#if !_DEBUG
				motion_ctrl.play(
					Utility::hexbytes2uint(m_buffer.data, 2)
				);
			#endif
		}

		void stopMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::stopMotion()"));
			#endif

			#if !_DEBUG
				motion_ctrl.stopping();
			#endif
		}

		void popCode()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::popCode()"));
			#endif

			#if !_DEBUG
				interpreter.popCode();
			#endif
		}

		void pushCode()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::pushCode()"));

				system.outputSerial().print(F("> slot : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> loop_count : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 2, 2));
			#endif

			#if !_DEBUG
				m_code_tmp.slot       = Utility::hexbytes2uint(m_buffer.data, 2);
				m_code_tmp.loop_count = Utility::hexbytes2uint(m_buffer.data + 2, 2);

				interpreter.pushCode(m_code_tmp);
			#endif
		}

		void resetInterpreter()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::resetInterpreter()"));
			#endif

			#if !_DEBUG
				interpreter.reset();
			#endif
		}

		void setHome()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setHome()"));

				system.outputSerial().print(F("> joint_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> angle : "));
				system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			#if !_DEBUG
				joint_ctrl.setHomeAngle(
					Utility::hexbytes2uint(m_buffer.data, 2),
					Utility::hexbytes2int(m_buffer.data + 2, 3)
				);
			#endif
		}

		void setJointSettings()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setJointSettings()"));
			#endif

			#if !_DEBUG
				joint_ctrl.resetSettings();
			#endif
		}

		void setMax()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMax()"));

				system.outputSerial().print(F("> joint_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> angle : "));
				system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			#if !_DEBUG
				joint_ctrl.setMaxAngle(
					Utility::hexbytes2uint(m_buffer.data, 2),
					Utility::hexbytes2int(m_buffer.data + 2, 3)
				);
			#endif
		}

		void setMotionFrame()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMotionFrame()"));

				system.outputSerial().print(F("> slot : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> frame_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 2, 2));

				system.outputSerial().print(F("> transition_time_ms : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 4, 4));

				for (int device_id = 0; device_id < joint_ctrl.SUM(); device_id++)
				{
					system.outputSerial().print(F("> output["));
					system.outputSerial().print(device_id);
					system.outputSerial().print(F("] : "));
					system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 8 + device_id * 4, 4));
				}
			#endif

			#if !_DEBUG
				m_frame_tmp.transition_time_ms = Utility::hexbytes2uint(m_buffer.data + 4, 4);

				for (int device_id = 0; device_id < joint_ctrl.SUM(); device_id++)
				{
					m_frame_tmp.joint_angle[device_id] = Utility::hexbytes2int(m_buffer.data + 8 + device_id * 4, 4);
				}

				if (m_installing == true)
				{
					if (m_frame_tmp.index < m_header_tmp.frame_length)
					{
						motion_ctrl.setFrame(m_header_tmp.slot, &m_frame_tmp);
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
						transition();

						readByte('m');
						readByte('f');
						accept();
						transition();

						readByte('0'); // dummy
						readByte('0'); // dummy
						readByte('0'); // dummy
						readByte('0'); // dummy
					}
				}
				else
				{
					m_frame_tmp.index = Utility::hexbytes2uint(m_buffer.data + 2, 2);
					motion_ctrl.setFrame(Utility::hexbytes2uint(m_buffer.data, 2), &m_frame_tmp);
				}
			#endif
		}

		void setMotionHeader()
		{
			strncpy(m_header_tmp.name, m_buffer.data + 2, 20);
			m_header_tmp.name[20] = '\0';
			m_buffer.data[2] = '\0';

			if (   !(m_purser[ARGUMENTS_INCOMING]->purse(m_buffer.data))
				|| !(m_purser[ARGUMENTS_INCOMING]->purse(m_buffer.data + 22)) )
			{
				return;
			}


			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMotionHeader()"));

				system.outputSerial().print(F("> slot : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> name : "));
				system.outputSerial().println(m_header_tmp.name);

				system.outputSerial().print(F("> func : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 22, 2));

				system.outputSerial().print(F("> arg0 : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 24, 2));

				system.outputSerial().print(F("> arg1 : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 26, 2));

				system.outputSerial().print(F("> frame_length : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data + 28, 2));
			#endif

			#if !_DEBUG
				m_header_tmp.slot = Utility::hexbytes2uint(m_buffer.data, 2);
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

				motion_ctrl.setHeader(&m_header_tmp);

				if (m_installing == true)
				{
					readByte('>');
					accept();
					transition();

					readByte('m');
					readByte('f');
					accept();
					transition();

					readByte('0'); // dummy
					readByte('0'); // dummy
					readByte('0'); // dummy
					readByte('0'); // dummy

					m_frame_tmp.index = 0;
				}
			#endif
		}

		void setMin()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMin()"));

				system.outputSerial().print(F("> joint_id : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));

				system.outputSerial().print(F("> angle : "));
				system.outputSerial().println(Utility::hexbytes2int(m_buffer.data + 2, 3));
			#endif

			#if !_DEBUG
				joint_ctrl.setMinAngle(
					Utility::hexbytes2uint(m_buffer.data, 2),
					Utility::hexbytes2int(m_buffer.data + 2, 3)
				);
			#endif
		}

		void getJointSettings()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getJointSettings()"));
			#endif

			#if !_DEBUG
				joint_ctrl.dump();
			#endif
		}

		void getMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getMotion()"));

				system.outputSerial().print(F("> slot : "));
				system.outputSerial().println(Utility::hexbytes2uint(m_buffer.data, 2));
			#endif

			#if !_DEBUG
				motion_ctrl.dump(
					Utility::hexbytes2uint(m_buffer.data, 2)
				);
			#endif
		}

		void getVersionInformation()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getVersionInformation()"));
			#endif

			#if !_DEBUG
				system.dump();
			#endif
		}

	public:
		virtual void afterFook()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in function : Application::afterFook()"));
			#endif

			if (m_state == READY)
			{
				unsigned char header_id = m_purser[READY]->index();
				unsigned char cmd_id    = m_purser[COMMAND_INCOMING]->index();

				(this->*EVENT_HANDLER[header_id][cmd_id])();

				#if _ENSOUL_PLEN2
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

	Application app_ctrl;
}


/*!
	@brief 初期化メソッド

	必要な初期化処理はこちらに記述してください。

	@attention
	デジタルピンの出力は初期レベルを与えない場合不定となります。
	モード設定を行うようなピンの初期化は必ず行ってください。
*/
void setup()
{
	randomSeed(analogRead(PLEN2::Pin::RANDOM_DEVCIE_IN()));

	joint_ctrl.loadSettings();

	#if _ENSOUL_PLEN2
		/*!
			@attention
			バスを介してセンサ値を取得する場合、通信対象のファームウェアが
			完全に立ち上がるのを待つ必要があります。
		*/
		delay(3000);
	#endif
}


/*!
	@brief メインポーリングループ

	繰り返し実行したい処理はこちらに記述してください。

	@attention
	動作を強制終了したい場合、exit()命令を使用する必要があります。
	return命令では、再度ループが実行されてしまいます。
*/
void loop()
{
	if (motion_ctrl.playing())
	{
		if (motion_ctrl.frameUpdatable())
		{
			motion_ctrl.frameUpdate();
		}

		if (motion_ctrl.frameUpdateFinished())
		{
			if (motion_ctrl.nextFrameLoadable())
			{
				motion_ctrl.loadNextFrame();

				/*!
					@attention
					オペレーションテストでは発生しない、本構成のみにおいて発生するインタプリタ実行時の
					モーションフレームオーバーフローを回避するために必須！
				*/
				if (   (motion_ctrl.playing() == false)
					&& interpreter.ready() )
				{
					interpreter.popCode();
				}
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

	if (system.USBSerial().available())
	{
		app_ctrl.readByte(system.USBSerial().read());

		if (app_ctrl.accept())
		{
			app_ctrl.transition();

		}
	}

	if (system.BLESerial().available())
	{
		app_ctrl.readByte(system.BLESerial().read());

		if (app_ctrl.accept())
		{
			app_ctrl.transition();
		}
	}

	#if _ENSOUL_PLEN2
		soul.logging();
		soul.action();
	#endif
}
