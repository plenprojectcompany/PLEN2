#include <string.h>

#include <Wire.h>
#include <EEPROM.h>
#include "System.h"
#include "JointController.h"
#include "MotionController.h"

namespace
{
	PLEN2::System           system;
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
}


/*!
	@brief Application Entry Point
*/
void setup()
{
	while (!Serial);
	system.outputSerial().println("Reset now...");

	PLEN2::MotionController::Header header;
	strncpy(header.name, "Empty", 6);
	header.frame_length  = 1;
	header.use_extra     = 0;
	header.use_jump      = 0;
	header.use_loop      = 0;
	header.loop_begin    = 0;
	header.loop_end      = 0;
	header.loop_count    = 255;
	header.stop_flags[0] = 255;
	header.stop_flags[1] = 255;
	header.stop_flags[2] = 255;

	PLEN2::MotionController::Frame frame;
	frame.index              = 0;
	frame.transition_time_ms = 100;
	for (int joint_id = 0; joint_id < PLEN2::JointController::SUM(); joint_id++)
	{
		frame.joint_angle[joint_id] = 0;
	}
	for (int index = 0; index < 8; index++)
	{
		frame.device_value[index] = 0;
	}

	for (
		int slot = PLEN2::MotionController::Header::SLOT_BEGIN();
		slot < PLEN2::MotionController::Header::SLOT_END();
		slot++
	)
	{
		header.slot = slot;
		header.jump_slot = slot;

		motion_ctrl.setHeader(&header);
		motion_ctrl.setFrame(slot, &frame);
	}

	system.outputSerial().println("Completed!");
}

void loop()
{
	// noop.
}
