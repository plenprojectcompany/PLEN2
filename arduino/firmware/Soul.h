/*!
    @file      Soul.h
    @brief     Provide the class which makes natural moving, for PLEN.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_SOUL_H
#define PLEN2_SOUL_H


#include <stdint.h>

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
    enum { SLOT_GETUP_FACE_UP     = 88    };

    //! @brief Slot of get up (face down) motion
    enum { SLOT_GETUP_FACE_DOWN   = 89    };

    //! @brief Beginning slot of random motion patterns
    enum { MOTIONS_SLOT_BEGIN     = 83    };

    //! @brief Ending slot of random motion patterns
    enum { MOTIONS_SLOT_END       = 88    };

    //! @brief The interval which makes stated periods
    enum { BASE_INTERVAL_MSEC     = 15000 };

    //! @brief The interval which gives flicker to base-interval
    enum { RANDOM_INTERVAL_MSEC   = 10000 };

    //! @brief Wait time for getting up automatically
    enum { GETUP_WAIT_MSEC        = 1000  };

    //! @brief Sampling interval
    enum { SAMPLING_INTERVAL_MSEC = 100   };

    //! @brief A threshold to decide gravity axis
    enum { GRAVITY_AXIS_THRESHOLD = 13000 };

    void m_preprocess();


    uint32_t m_before_user_action_msec;
    uint32_t m_next_sampling_msec;

    uint32_t m_action_interval;

    bool m_lying;

    uint8_t m_log_count;

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