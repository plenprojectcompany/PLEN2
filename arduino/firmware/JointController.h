/*!
	@file      JointController.h
	@brief     Management class of joints.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef PLEN2_JOINT_CONTROLLER_H
#define PLEN2_JOINT_CONTROLLER_H

namespace PLEN2
{
	class JointController;
}

/*!
	@brief Management class of joints

	In Atmega32u4, one timer can output 3 PWM signals at a time.
	The MCU can control 24 servos by connecting 3bit multiplexer in each signal output lines.

	@note
	There is information about PLEN1.4's configuration below.
	@code
	enum {
		SUM = 24, //!< Summation of the servos controllable.

		ANGLE_MIN     = -600, //!< Min angle of the servos.
		ANGLE_MAX     =  600, //!< Max angle of the servos.
		ANGLE_NEUTRAL =    0  //!< Neutral angle of the servos.
	};

	//! @brief PWM width that to make min angle
	inline static const int PWM_MIN()     { return 492;  }

	//! @brief PWM width that to make max angle
	inline static const int PWM_MAX()     { return 816;  }

	//! @brief PWM width that to make neutral angle
	inline static const int PWM_NEUTRAL() { return 654;  }
	@endcode
*/
class PLEN2::JointController
{
public:
	enum {
		SUM = 24, //!< Summation of the servos controllable.

		ANGLE_MIN     = -700, //!< Min angle of the servos.
		ANGLE_MAX     =  700, //!< Max angle of the servos.
		ANGLE_NEUTRAL =    0  //!< Neutral angle of the servos.
	};

private:
	//! @brief Initialized flag's address on internal EEPROM
	inline static const int INIT_FLAG_ADDRESS()     { return 0; }

	//! @brief Initialized flag's value
	inline static const int INIT_FLAG_VALUE()       { return 2; }

	//! @brief Head-address of joint settings on internal EEPROM
	inline static const int SETTINGS_HEAD_ADDRESS() { return 1; }

	/*!
		@brief Management class of joint setting
	*/
	class JointSetting
	{
	public:
		int MIN;  //!< Setting about min angle.
		int MAX;  //!< Setting about max angle.
		int HOME; //!< Setting about home angle.

		/*!
			@brief Constructor
		*/
		JointSetting()
			: MIN(ANGLE_MIN)
			, MAX(ANGLE_MAX)
			, HOME(ANGLE_NEUTRAL)
		{
			// noop.
		}
	};

	JointSetting m_SETTINGS[SUM];

public:
	/*!
		@brief Management class (as namespace) of multiplexer

		@note
		The methods the class has are more accurate that there are in the namespace named Multiplexer,
		but C++'s idiom doesn't accept syntax that described before.
	*/
	class Multiplexer {
	public:
		//! @brief Summation of multiplexers
		inline static const int SUM()              { return 3; }

		//! @brief Number of controllable lines by a multiplexer
		inline static const int SELECTABLE_LINES() { return 8; }
	};

	//! @brief PWM width that to make min angle
	inline static const int PWM_MIN()     { return 480;  }

	//! @brief PWM width that to make max angle
	inline static const int PWM_MAX()     { return 820;  }

	//! @brief PWM width that to make neutral angle
	inline static const int PWM_NEUTRAL() { return 650;  }

	/*!
		@brief Finished flag of PWM output procedure 1 cycle

		@attention
		The instance should be a private member normally.
		It is a public member because it is only way to access from Timer 1 overflow interruption vector,
		so you must not access it from other functions basically.
	*/
	volatile static bool m_1cycle_finished;

	/*!
		@brief PWM buffer

		@attention
		The instance should be a private member normally.
		It is a public member because it is only way to access from Timer 1 overflow interruption vector,
		so you must not access it from other functions basically.
	*/
	static unsigned int m_pwms[SUM];

	/*!
		@brief Constructor
	*/
	JointController();

	/*!
		@brief Load the joint settings

		The method reads joint settings from internal EEPROM.
		If the EEPROM has no settings, the method also writes the default values.

		@sa
		JointController.cpp::Shared::m_SETTINGS_INITIAL

		@attention
		The method should call in constructor normally,
		but initialized timing of any interruption is indefinite, so might get deadlock.
		(The method uses serial communication and internal EEPROM accessing, so it happens interruption.)
	*/
	void loadSettings();

	/*!
		@brief Reset the joint settings

		Write default settings to internal EEPROM.
	*/
	void resetSettings();

	/*!
		@brief Get min angle of the joint given

		@param [in] joint_id Please set joint id you want to get min angle.

		@return Reference of min angle a joint expressed by **joint_id** has.
		@retval -32768 Argument error. (**joint_id** is invalid.)
	*/
	const int& getMinAngle(unsigned char joint_id);

	/*!
		@brief Get max angle of the joint given

		@param [in] joint_id Please set joint id you want to get max angle.

		@return Reference of max angle a joint expressed by **joint_id** has.
		@retval -32768 Argument error. (**joint_id** is invalid.)
	*/
	const int& getMaxAngle(unsigned char joint_id);

	/*!
		@brief Get home angle of the joint given

		@param [in] joint_id Please set joint id you want to get home angle.

		@return Reference of home angle a joint expressed by **joint_id** has.
		@retval -32768 Argument error. (**joint_id** is invalid.)
	*/
	const int& getHomeAngle(unsigned char joint_id);

	/*!
		@brief Set min angle of the joint given

		@param [in] joint_id Please set joint id you want to define min angle.
		@param [in] angle    Please set angle that has steps of degree 1/10.

		@return Result
	*/
	bool setMinAngle(unsigned char joint_id, int angle);

	/*!
		@brief Set max angle of the joint given

		@param [in] joint_id Please set joint id you want to define max angle.
		@param [in] angle    Please set angle that has steps of degree 1/10.

		@return Result
	*/
	bool setMaxAngle(unsigned char joint_id, int angle);

	/*!
		@brief Set home angle of the joint given

		@param [in] joint_id Please set joint id you want to define home angle.
		@param [in] angle    Please set angle that has steps of degree 1/10.

		@return Result
	*/
	bool setHomeAngle(unsigned char joint_id, int angle);

	/*!
		@brief Set angle of the joint given

		@param [in] joint_id Please set joint id you want to set angle.
		@param [in] angle    Please set angle that has steps of degree 1/10.

		@return Result

		@attention
		<b>angle</b> might not be setting actually.
		It is setting after trimming by user defined min-max value or servo's range,
		so please consider it when writing a unit test.
	*/
	bool setAngle(unsigned char joint_id, int angle);

	/*!
		@brief Set angle to "angle-diff + home-angle" of the joint given

		@param [in] joint_id   Please set joint id you want to set angle-diff.
		@param [in] angle_diff Please set angle-diff that has steps of degree 1/10.

		@return Result

		@attention
		<b>angle_diff</b> might not be setting actually.
		It is setting after trimming by user defined min-max value or servo's range,
		so please consider it when writing a unit test.
	*/
	bool setAngleDiff(unsigned char joint_id, int angle_diff);

	/*!
		@brief Dump the joint settings

		Output result like JSON format below.
		@code
		[
			{
				"max": <integer>,
				"min": <integer>,
				"home": <integer>
			},
			...
		]
		@endcode
	*/
	void dump();
};

#endif // PLEN2_JOINT_CONTROLLER_H
