/*!
	@file      Soul.h
	@brief     Provide the class which makes natural moving, for PLEN.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_SOUL_H
#define PLEN2_SOUL_H


namespace PLEN2
{
	class AccelerationGyroSensor;
	class MotionController;

	class Soul;
}

/*!
	@brief The class which makes natural moving, for PLEN
*/
class PLEN2::Soul
{
private:
	//! @brief Slot of get up (face up) motion
	inline static const int SLOT_GETUP_FACE_UP()     { return 88;    }

	//! @brief Slot of get up (face down) motion
	inline static const int SLOT_GETUP_FACE_DOWN()   { return 89;    }

	//! @brief Beginning slot of random motion patterns
	inline static const int MOTIONS_SLOT_BEGIN()     { return 83;    }

	//! @brief Ending slot of random motion patterns
	inline static const int MOTIONS_SLOT_END()       { return 88;    }

	//! @brief The interval which makes stated periods
	inline static const int BASE_INTERVAL_MSEC()     { return 15000; }

	//! @brief The interval which gives flicker to base-interval
	inline static const int RANDOM_INTERVAL_MSEC()   { return 10000; }

	//! @brief Wait time for getting up automatically
	inline static const int GETUP_WAIT_MSEC()        { return 2000;  }

	//! @brief Sampling interval
	inline static const int SAMPLING_INTERVAL_MSEC() { return 100;   }

	//! @brief A threshold to decide gravity axis
	inline static const int GRAVITY_AXIS_THRESHOLD() { return 13000; }

	void m_preprocess();


	unsigned long m_before_user_action_msec;
	unsigned long m_next_sampling_msec;

	unsigned long m_action_interval;

	bool m_lying;

	unsigned char m_log_count;

	AccelerationGyroSensor* m_sensor_ptr;
	MotionController*       m_motion_ctrl_ptr;

public:
	/*!
		@brief Constructor

		@param [in, out] sensor      An instance of the sensor class.
		@param [in, out] motion_ctrl An instance of the motion controller class.
	*/
	Soul(AccelerationGyroSensor& sensor, MotionController& motion_ctrl);

	/*!
		@brief Log PLEN's state
	*/
	void log();

	/*!
		@brief Log a timing when a user action is input

		Please run the method at the all timings when user action is input.
	*/
	void userActionInputed();

	/*!
		@brief Apply appropriate motion based on logging state
	*/
	void action();
};

#endif // PLEN2_SOUL_H