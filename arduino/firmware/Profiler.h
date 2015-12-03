/*!
	@file      Profiler.h
	@brief     Tiny metrics class for Arduino.
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#pragma once

#ifndef UTILITY_PROFILER_H
#define UTILITY_PROFILER_H


class __FlashStringHelper;


namespace Utility
{
	class Profiler;
}

/*!
	@brief Tiny metrics class

	Refer to the usage below.
	@code
	void anyFunction()
	{
		// When instantiate the class, it outputs message given.
		volatile Utility::Profiler p(F("anyFunction()"));

		Any code here...

		// Outputting metrics about this function when profiler instance has destroyed.
	}
	@endcode
*/
class Utility::Profiler
{
private:
	unsigned long m_begin;
	unsigned long m_end;

	void m_tabbing();

	// Disable copy constructor and operator =.
	Profiler(const Profiler&);
	Profiler& operator=(const Profiler&);

public:
	/*!
		@brief Constructor

		@param [in] fsh_ptr Please set startup message.

		@attention
		Arduino IDE is using optimization option -Os,
		so you should use volatile prefix when instantiate the class.
	*/
	Profiler(const __FlashStringHelper* fsh_ptr);

	/*!
		@brief Destructor
	*/
	~Profiler();
};

#endif // UTILITY_PROFILER_H
