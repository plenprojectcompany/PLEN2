/*!
    @file      JointController.h
    @brief     Management class of joints.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_JOINT_CONTROLLER_H
#define PLEN2_JOINT_CONTROLLER_H


#include <stdint.h>

#include "BuildConfig.h"

namespace PLEN2
{
    class JointController;
}

/*!
    @brief Management class of joints

    In Atmega32u4, one timer can output 3 PWM signals at a time.
    The MCU can control 24 servos by connecting 3bit multiplexer in each signal output lines.
*/
class PLEN2::JointController
{
public:
    enum JOINT_INDEX
    {
        LEFT_SHOULDER_PITCH  = 0,
        LEFT_THIGH_YAW       = 1,
        LEFT_SHOULDER_ROLL   = 2,
        LEFT_ELBOW_ROLL      = 3,
        LEFT_THIGH_ROLL      = 4,
        LEFT_THIGH_PITCH     = 5,
        LEFT_KNEE_PITCH      = 6,
        LEFT_FOOT_PITCH      = 7,
        LEFT_FOOT_ROLL       = 8,
        RIGHT_SHOULDER_PITCH = 12,
        RIGHT_THIGH_YAW      = 13,
        RIGHT_SHOULDER_ROLL  = 14,
        RIGHT_ELBOW_ROLL     = 15,
        RIGHT_THIGH_ROLL     = 16,
        RIGHT_THIGH_PITCH    = 17,
        RIGHT_KNEE_PITCH     = 18,
        RIGHT_FOOT_PITCH     = 19,
        RIGHT_FOOT_ROLL      = 20
    };

    enum BASIC_JOINT_SETTINGS
    {
        JOINTS_SUM = 24, //!< Summation of the servos controllable.

        #if TARGET_PLEN14
            ANGLE_MIN     = -600, //!< Min angle of the servos.
            ANGLE_MAX     =  600, //!< Max angle of the servos.
            ANGLE_NEUTRAL =    0  //!< Neutral angle of the servos.
        #endif

        #if TARGET_PLEN20
            ANGLE_MIN     = -700, //!< Min angle of the servos.
            ANGLE_MAX     =  700, //!< Max angle of the servos.
            ANGLE_NEUTRAL =    0  //!< Neutral angle of the servos.
        #endif
    };

private:
    //! @brief Initialized flag's address on internal EEPROM
    enum { INIT_FLAG_ADDRESS = 0 };

    //! @brief Initialized flag's value
    enum { INIT_FLAG_VALUE = 2 };

    //! @brief Head-address of joint settings on internal EEPROM
    enum { SETTINGS_HEAD_ADDRESS = 1 };

    /*!
        @brief Management class of joint setting
    */
    class JointSetting
    {
    public:
        int16_t MIN;  //!< Setting about min angle.
        int16_t MAX;  //!< Setting about max angle.
        int16_t HOME; //!< Setting about home angle.

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

    JointSetting m_SETTINGS[JOINTS_SUM];

public:
    /*!
        @brief Management class (as namespace) of multiplexer

        @note
        The methods of the class are more accurate than those in the namespace named Multiplexer,
        but C++'s idiom doesn't accept the syntax that was described before.
    */
    class Multiplexer
    {
    public:
        //! @brief Summation of multiplexers
        enum { SUM = 3 };

        //! @brief Number of controllable lines by a multiplexer
        enum { SELECTABLE_LINES = 8 };
    };

    #if TARGET_PLEN14
        //! @brief PWM width that to make min angle
        enum { PWM_MIN = 492 };

        //! @brief PWM width that to make max angle
        enum { PWM_MAX = 816 };

        //! @brief PWM width that to make neutral angle
        enum { PWM_NEUTRAL = 654 };
    #endif

    #if TARGET_PLEN20
        //! @brief PWM width that to make min angle
        enum { PWM_MIN = 480 };

        //! @brief PWM width that to make max angle
        enum { PWM_MAX = 820 };

        //! @brief PWM width that to make neutral angle
        enum { PWM_NEUTRAL = 650 };
    #endif

    /*!
        @brief Finished flag of PWM output procedure 1 cycle

        @attention
        The instance should be a private member normally.
        It is a public member because it is the only way to access it from Timer 1 overflow interruption vector,
        so you must not access it from other functions basically.
    */
    volatile static bool m_1cycle_finished;

    /*!
        @brief PWM buffer

        @attention
        The instance should be a private member normally.
        It is a public member because it is the only way to access it from Timer 1 overflow interruption vector,
        so you must not access it from other functions basically.
    */
    static uint16_t m_pwms[JOINTS_SUM];

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
        The method should be called in constructor normally,
        but initialized timing of any interruption is indefinite, so it might get deadlocked.
        (The method uses serial communication and internal EEPROM access, so interruption happens.)
    */
    void loadSettings();

    /*!
        @brief Reset the joint settings

        Write default settings to internal EEPROM.
    */
    void resetSettings();

    /*!
        @brief Get min angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to get min angle.

        @return Reference of min angle of a joint described by **joint_id**.
        @retval -32768 Argument error. (**joint_id** is invalid.)
    */
    const int16_t& getMinAngle(uint8_t joint_id);

    /*!
        @brief Get max angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to get max angle.

        @return Reference of max angle of a joint described by **joint_id**.
        @retval -32768 Argument error. (**joint_id** is invalid.)
    */
    const int16_t& getMaxAngle(uint8_t joint_id);

    /*!
        @brief Get home angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to get home angle.

        @return Reference of home angle a joint expressed by **joint_id** has.
        @retval -32768 Argument error. (**joint_id** is invalid.)
    */
    const int16_t& getHomeAngle(uint8_t joint_id);

    /*!
        @brief Set min angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to define the min angle.
        @param [in] angle    Please set angle that has steps of degree 1/10.

        @return Result
    */
    bool setMinAngle(uint8_t joint_id, int16_t angle);

    /*!
        @brief Set max angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to define the max angle.
        @param [in] angle    Please set angle that has steps of degree 1/10.

        @return Result
    */
    bool setMaxAngle(uint8_t joint_id, int16_t angle);

    /*!
        @brief Set home angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to define the home angle.
        @param [in] angle    Please set angle that has steps of degree 1/10.

        @return Result
    */
    bool setHomeAngle(uint8_t joint_id, int16_t angle);

    /*!
        @brief Set angle of the joint given

        @param [in] joint_id Please set the joint id from which you want to set the angle.
        @param [in] angle    Please set angle that has steps of degree 1/10.

        @return Result

        @attention
        <b>angle</b> might not be set actually.
        It is set after trimming by user defined min-max value or servo's range,
        so please consider it when writing a unit test.
    */
    bool setAngle(uint8_t joint_id, int16_t angle);

    /*!
        @brief Set angle to "angle-diff + home-angle" of the joint given

        @param [in] joint_id   Please set the joint id from which you want to set the angle-diff.
        @param [in] angle_diff Please set angle-diff that has steps of degree 1/10.

        @return Result

        @attention
        <b>angle_diff</b> might not be set actually.
        It is set after trimming by user defined min-max value or servo's range,
        so please consider it when writing a unit test.
    */
    bool setAngleDiff(uint8_t joint_id, int16_t angle_diff);

    /*!
        @brief Dump the joint settings

        Output result in JSON format as below.
        @code
        [
            {
                "@device": <integer>,
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
