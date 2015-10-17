/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

// Arduinoライブラリ
#include "Arduino.h"

// 独自ライブラリ
#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"
#include "JointController.h"
#include "MotionController.h"
#include "Soul.h"


namespace
{
	enum {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		EOE
	};

	long acc_backup[EOE] = { 0 };

	PLEN2::System system;
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


void PLEN2::Soul::logging()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Soul::logging()"));
	#endif

	static unsigned char call_count = 0;

	/*!
		@note
		通常、PLENが横たわっていると判定された後、即座に復帰処理が行われるため、
		(*)の条件により以下のif文内部が実行されることはない。
	*/
	if (   (m_lying == true) // (*) sanity check.
		|| (millis() < m_next_sampling_msec)
		|| m_motion_ctrl_ptr->playing() )
	{
		return;
	}

	/*!
		@note
		既にいずれかのシリアルへデータが到着している場合、
		コマンドラインの撹乱を防ぐために処理を抜ける。
	*/
	if (   system.BLESerial().available()
		|| system.USBSerial().available() )
	{
		return;
	}

	m_sensor_ptr->sampling();

	acc_backup[X_AXIS] += m_sensor_ptr->getAccX();
	acc_backup[Y_AXIS] += m_sensor_ptr->getAccY();
	acc_backup[Z_AXIS] += m_sensor_ptr->getAccZ();

	m_next_sampling_msec += SAMPLING_INTERVAL_MSEC();
	call_count++;

	if (call_count == (GETUP_WAIT_MSEC() / SAMPLING_INTERVAL_MSEC()))
	{
		acc_backup[X_AXIS] /= (GETUP_WAIT_MSEC() / SAMPLING_INTERVAL_MSEC());
		acc_backup[Y_AXIS] /= (GETUP_WAIT_MSEC() / SAMPLING_INTERVAL_MSEC());
		acc_backup[Z_AXIS] /= (GETUP_WAIT_MSEC() / SAMPLING_INTERVAL_MSEC());

		if (   (abs(acc_backup[Y_AXIS]) > abs(acc_backup[X_AXIS]))
			&& (abs(acc_backup[Y_AXIS]) > abs(acc_backup[Z_AXIS]))
			&& (abs(acc_backup[Y_AXIS]) > GRAVITY_AXIS_THRESHOLD()) )
		{
			m_lying = true;
		}
		else
		{
			acc_backup[X_AXIS] = 0;
			acc_backup[Y_AXIS] = 0;
			acc_backup[Z_AXIS] = 0;
		}

		call_count = 0;
	}
}


void PLEN2::Soul::userActionInputed()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Soul::userActionInputed()"));
	#endif

	m_before_user_action_msec = millis();
}


void PLEN2::Soul::action()
{
	#if _DEBUG
		system.outputSerial().println(F("=== running in function : Soul::action()"));
	#endif

	if (m_lying == true)
	{
		if (acc_backup[Y_AXIS] < 0)
		{
			m_motion_ctrl_ptr->play(SLOT_GETUP_FACE_DOWN());
		}
		else
		{
			m_motion_ctrl_ptr->play(SLOT_GETUP_FACE_UP());
		}

		m_lying = false;

		acc_backup[X_AXIS] = 0;
		acc_backup[Y_AXIS] = 0;
		acc_backup[Z_AXIS] = 0;

		return;
	}

	if (millis() - m_before_user_action_msec > m_action_interval)
	{
		m_motion_ctrl_ptr->play(random(MOTIONS_SLOT_BEGIN(), MOTIONS_SLOT_END()));

		m_before_user_action_msec = millis();
		m_action_interval = BASE_INTERVAL_MSEC() + random(RANDOM_INTERVAL_MSEC());
	}
}