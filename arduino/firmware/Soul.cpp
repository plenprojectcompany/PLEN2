/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define DEBUG false

#include <Arduino.h>

#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"
#include "Soul.h"

#if DEBUG
	#include "Profiler.h"
#endif


namespace
{
	enum {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		EOE
	};

	namespace Shared
	{
		long acc_backup[EOE] = { 0 };
	}
}


PLEN2::Soul::Soul(AccelerationGyroSensor& sensor, MotionController& motion_ctrl)
{
	m_sensor_ptr      = &sensor;
	m_motion_ctrl_ptr = &motion_ctrl;

	m_before_user_action_msec = 0;
	m_next_sampling_msec      = SAMPLING_INTERVAL_MSEC();

	m_action_interval = BASE_INTERVAL_MSEC() + random(RANDOM_INTERVAL_MSEC());

	m_lying = false;
}


void PLEN2::Soul::m_preprocess()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Soul::m_preprocess()"));
	#endif


	if (m_log_count >= (GETUP_WAIT_MSEC() / SAMPLING_INTERVAL_MSEC()))
	{
		Shared::acc_backup[X_AXIS] /= m_log_count;
		Shared::acc_backup[Y_AXIS] /= m_log_count;
		Shared::acc_backup[Z_AXIS] /= m_log_count;

		if (   (abs(Shared::acc_backup[Y_AXIS]) > abs(Shared::acc_backup[X_AXIS]))
			&& (abs(Shared::acc_backup[Y_AXIS]) > abs(Shared::acc_backup[Z_AXIS]))
			&& (abs(Shared::acc_backup[Y_AXIS]) > GRAVITY_AXIS_THRESHOLD()) )
		{
			m_lying = true;
		}
		else
		{
			Shared::acc_backup[X_AXIS] = 0;
			Shared::acc_backup[Y_AXIS] = 0;
			Shared::acc_backup[Z_AXIS] = 0;
		}

		m_log_count = 0;
	}
}


void PLEN2::Soul::log()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Soul::log()"));
	#endif


	if (   m_motion_ctrl_ptr->playing()
		|| (millis() < m_next_sampling_msec) )
	{
		return;
	}

	/*!
		@note
		If something data arrived in the serial buffer already,
		return the method to keep the integrity of the data.
	*/
	if (   System::BLESerial().available()
		|| System::USBSerial().available() )
	{
		return;
	}


	m_sensor_ptr->sampling();

	Shared::acc_backup[X_AXIS] += m_sensor_ptr->getAccX();
	Shared::acc_backup[Y_AXIS] += m_sensor_ptr->getAccY();
	Shared::acc_backup[Z_AXIS] += m_sensor_ptr->getAccZ();

	m_next_sampling_msec += SAMPLING_INTERVAL_MSEC();
	m_log_count++;

	m_preprocess();
}


void PLEN2::Soul::userActionInputed()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Soul::userActionInputed()"));
	#endif

	m_before_user_action_msec = millis();
}


void PLEN2::Soul::action()
{
	#if DEBUG
		volatile Utility::Profiler p(F("Soul::action()"));
	#endif

	if (m_lying)
	{
		if (Shared::acc_backup[Y_AXIS] > 0)
		{
			m_motion_ctrl_ptr->play(SLOT_GETUP_FACE_DOWN());
		}
		else
		{
			m_motion_ctrl_ptr->play(SLOT_GETUP_FACE_UP());
		}

		m_lying = false;

		Shared::acc_backup[X_AXIS] = 0;
		Shared::acc_backup[Y_AXIS] = 0;
		Shared::acc_backup[Z_AXIS] = 0;

		return;
	}

	if (millis() - m_before_user_action_msec > m_action_interval)
	{
		m_motion_ctrl_ptr->play(random(MOTIONS_SLOT_BEGIN(), MOTIONS_SLOT_END()));

		m_before_user_action_msec = millis();
		m_action_interval = BASE_INTERVAL_MSEC() + random(RANDOM_INTERVAL_MSEC());
	}
}