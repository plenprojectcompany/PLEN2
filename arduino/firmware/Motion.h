/*!
	@file      Motion.h
	@brief     Provide motion components.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef PLEN2_MOTION_H
#define PLEN2_MOTION_H

#include "JointController.h"


namespace PLEN2
{
	namespace Motion
	{
		enum {
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
	so if you change the order of member instances, PLEN does not work properly
	if you did not re-install all motions.
*/
class PLEN2::Motion::Header
{
public:
	enum {
		/*!
			@brief Length of a motion name.

			@attention
			Regard end of a string as '\0', so the actual length is 20 bytes.
		*/
		NAME_LENGTH     = 21,

		FRAMELENGTH_MIN =  1, //!< Minimum value of frame length. 
		FRAMELENGTH_MAX = 20  //!< Maximum value of frame length.
	};

	void init();

	/*!
		@brief Write the header to external EEPROM

		@return Result
	*/
	bool set();

	/*!
		@brief Read the header from external EEPROM

		@return Result
	*/
	bool get();


	unsigned char slot;              //!< Slot number of a motion.
	char          name[NAME_LENGTH]; //!< Motion name.
	unsigned char frame_length;      //!< Frame length of a motion.

	unsigned char NON_RESERVED : 5;  //!< Undefined area. (It is reserved for future changes.)
	unsigned char use_extra    : 1;  //!< Selector for to enable "extra".
	unsigned char use_jump     : 1;  //!< Selector for to enable "jump".
	unsigned char use_loop     : 1;  //!< Selector for to enable "loop".

	unsigned char loop_begin   : 4;  //!< Frame number of loop's beginning.
	unsigned char loop_end     : 4;  //!< Frame number of loop's ending.

	unsigned char loop_count;        //!< Loop count. (Using 255 as infinity.)
	unsigned char jump_slot;         //!< Slot number that is used for jumpping when play the motion finished.
	unsigned char stop_flags[3];     //!< Array of stop flags.
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
	enum {
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
		@brief Write the frame to external EEPROM

		@param [in] slot Slot number of a motion.

		@return Result
	*/
	bool set(unsigned char slot);

	/*!
		@brief Read the frame from external EEPROM

		@param [in] slot Slot number of a motion.

		@return Result
	*/
	bool get(unsigned char slot);


	unsigned char index;                             //!< Index of a frame.
	unsigned int  transition_time_ms;                //!< Time of transit to the frame.
	int           joint_angle[JointController::SUM]; //!< Angles.
	unsigned char device_value[8];                   //!< Output values.
};

#endif // PLEN2_MOTION_H