/*!
	@file      MotionController.h
	@brief     Management class of a motion.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_MOTION_CONTROLLER_H
#define PLEN2_MOTION_CONTROLLER_H

#include "JointController.h"


namespace PLEN2
{
	namespace Motion
	{
		class Header;
		class Frame;
	}

	#ifdef PLEN2_INTERPRETER_H
		class Interpreter;
	#endif

	class MotionController;
}

/*!
	@brief Management class of a motion
*/
class PLEN2::MotionController
{
#ifdef PLEN2_INTERPRETER_H
	friend class Interpreter;
#endif

public:
	/*!
		@brief Constructor

		@param [in] joint_ctrl An instance of joint controller.
	*/
	MotionController(JointController& joint_ctrl);

	/*!
		@brief Decide a motion is playing

		@return Result
	*/
	bool playing();

	/*!
		@brief Decide a motion frame is updatable

		@return Result
	*/
	bool frameUpdatable();

	/*!
		@brief Decide that updating a frame has finished

		@return Result
	*/
	bool updatingFinished();

	/*!
		@brief Decide there is a loadable frame at the next time

		@return Result
	*/
	bool nextFrameLoadable();

	/*!
		@brief Play a motion

		@param [in] slot  Number of a motion.
	*/
	void play(unsigned char slot);

	/*!
		@brief Will stop playing a motion

		The method doesn't stop playing a motion just after running itself,
		but will stop it when a frame that has the stop flag discovered.
	*/
	void willStop();

	/*!
		@brief Stop playing a motion
	*/
	void stop();

	/*
		@brief Add differences between current-frame and next-frame to current-frame
	*/
	void updateFrame();

	/*!
		@brief Load next frame
	*/
	void loadNextFrame();

	/*!
		@brief Dump a motion with JSON format

		Output JSON string like a "motion.json",
		however, property "device"'s type is not string but integer defined as device id in "device_map.json".

		@sa
		- https://github.com/plenproject/PLEN2/tree/master/motion
		- https://github.com/plenproject/plen__control_server/blob/master/control_server/device_map.json

		@param [in] slot Slot of a motion.
	*/
	void dump(unsigned char slot);

private:
	enum {
		FRAMEBUFFER_LENGTH = 2
	};

	void m_setupFrame(unsigned char index);
	void m_bufferingFrame();


	JointController* m_joint_ctrl_ptr;

	unsigned char m_transition_count;
	bool          m_playing;

	Motion::Header m_header;
	Motion::Frame  m_buffer[FRAMEBUFFER_LENGTH];
	Motion::Frame* m_frame_current_ptr;
	Motion::Frame* m_frame_next_ptr;

	long m_current_fixed_points[JointController::SUM];
	long m_diff_fixed_points[JointController::SUM];
};

#endif // PLEN2_MOTION_CONTROLLER_H