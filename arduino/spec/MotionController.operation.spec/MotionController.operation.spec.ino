#line 2 "MotionController.operation.spec.ino"


#include <Wire.h>
#include <EEPROM.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "Parser.h"
#include "Protocol.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"


namespace
{
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);


	class OperationTest: public PLEN2::Protocol
	{
	private:
		void playMotion()
		{
			motion_ctrl.play(
				Utility::hexbytes2uint(m_buffer.data, 2)
			);
		}

		void stopMotion()
		{
			motion_ctrl.willStop();
		}

		void getMotion()
		{
			motion_ctrl.dump(
				Utility::hexbytes2uint(m_buffer.data, 2)
			);
		}

	public:
		virtual void afterHook()
		{
			if (m_state == READY)
			{
				unsigned char header_id = m_parser[HEADER_INCOMING]->index();
				unsigned char cmd_id    = m_parser[COMMAND_INCOMING]->index();

				if ((header_id == 0) && (cmd_id == 5))
				{
					playMotion();
				}

				if ((header_id == 0) && (cmd_id == 6))
				{
					stopMotion();
				}

				if ((header_id == 3) && (cmd_id == 1))
				{
					getMotion();
				}
			}
		}
	};

	OperationTest test_core;
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	volatile PLEN2::System         system;
	volatile PLEN2::ExternalEEPROM exteeprom;

	joint_ctrl.loadSettings();


	while (!Serial); // for the Arduino Leonardo/Micro only.

	PLEN2::System::outputSerial().print(F("# Test : "));
	PLEN2::System::outputSerial().println(__FILE__);
}

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
			}
		}
	}

	if (PLEN2::System::USBSerial().available())
	{
		test_core.readByte(PLEN2::System::USBSerial().read());

		if (test_core.accept())
		{
			test_core.transitState();
		}
	}

	if (PLEN2::System::BLESerial().available())
	{
		test_core.readByte(PLEN2::System::BLESerial().read());

		if (test_core.accept())
		{
			test_core.transitState();
		}
	}

}