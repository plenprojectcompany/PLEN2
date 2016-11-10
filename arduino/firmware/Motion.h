/*!
    @file      Motion.h
    @brief     Provide motion components.
    @author    Kazuyuki TAKASE
    @copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_MOTION_H
#define PLEN2_MOTION_H


#include <stdint.h>

#include "JointController.h"

namespace PLEN2
{
    namespace Motion
    {
        enum
        {
            SLOT_BEGIN =  0, //!< Beginning value of slots.
            SLOT_END   = 90  //!< Ending value of slots.
        };

        class Header;
        class Frame;
    }
}


/*!
    @brief Class of a motion header

    @attention
    The firmware backs up memory allocation of an instance to external EEPROM,
    so if you change the order of the member instances, PLEN does not work properly
    if you did not re-install all motions.
*/
class PLEN2::Motion::Header
{
public:
    enum
    {
        /*!
            @brief Length of a motion name.

            @attention
            Regard end of a string as '\0', so the actual length is 20 bytes.
        */
        NAME_LENGTH     = 21,

        FRAMELENGTH_MIN =  1, //!< Minimum value of frame length.
        FRAMELENGTH_MAX = 20  //!< Maximum value of frame length.
    };

    /*!
        @brief Initialize the header

        @param [in, out] header An instance of header.
    */
    static void init(Header& header);

    /*!
        @brief Write the header to external EEPROM

        @param [in] slot Number of a header.
        @param [in] header An instance of header.

        @return Result
    */
    static bool set(uint8_t slot, const Header& header);

    /*!
        @brief Read the header from external EEPROM

        @param [in] slot Number of a header.
        @param [in, out] header An instance of header.

        @return Result
    */
    static bool get(uint8_t slot, Header& header);


    uint8_t slot;              //!< Slot number of a motion.
    char    name[NAME_LENGTH]; //!< Motion name.
    uint8_t frame_length;      //!< Frame length of a motion.

    uint8_t NON_RESERVED : 5;  //!< Undefined area. (It is reserved for future changes.)
    uint8_t use_extra    : 1;  //!< Selector to enable "extra".
    uint8_t use_jump     : 1;  //!< Selector to enable "jump".
    uint8_t use_loop     : 1;  //!< Selector to enable "loop".

    uint8_t loop_begin;        //!< Frame number of loop's beginning.
    uint8_t loop_end;          //!< Frame number of loop's ending.

    uint8_t loop_count;        //!< Loop count. (Using 255 as infinity.)
    uint8_t jump_slot;         //!< Slot number that is used for jumping when the motion's play is finished.

    /*!
        @brief Array of stop flags

        @todo
        Currently, the definition is uint8_t 2 bytes array,
        but in the future, it should be uint32_t variable.
        <br><br>
        Thus, the authors need to refactor I2C libraries.
    */
    uint8_t stop_flags[2];
};


/*!
    @brief Class of a motion frame

    @attention
    The firmware backs up memory allocation of an instance to external EEPROM,
    so if you change the order of member instances, PLEN does not work properly
    if you did not re-install all motions.
*/
class PLEN2::Motion::Frame
{
public:
    enum
    {
        /*!
            @brief Update interval between frames

            @sa
            Refer to ISR(TIMER1_OVF_vect), in JointController.cpp.
        */
        UPDATE_INTERVAL_MS = 32,

        FRAME_BEGIN =  0, //!< Beginning value of frames.
        FRAME_END   = 20  //!< Ending value of frames.
    };

    /*!
        @brief Initialize the frame

        @param [in, out] frame An instance of frame.
    */
    static void init(Frame& frame);

    /*!
        @brief Write the frame to external EEPROM

        @param [in] slot Slot number of a motion.
        @param [in] index Index of the frame.
        @param [in] frame An instance of frame.

        @return Result
    */
    static bool set(uint8_t slot, uint8_t index, const Frame& frame);

    /*!
        @brief Read the frame from external EEPROM

        @param [in] slot Slot number of a motion.
        @param [in] index Index of the frame.
        @param [in, out] frame An instance of frame.

        @return Result
    */
    static bool get(uint8_t slot, uint8_t index, Frame& frame);


    uint8_t  index;                                    //!< Index of a frame.
    uint16_t transition_time_ms;                       //!< Time of transit to the frame.
    int16_t  joint_angle[JointController::JOINTS_SUM]; //!< Angles.

    /*
    uint8_t  device_value[8];                          //!< Output values.
    */
};

#endif // PLEN2_MOTION_H
