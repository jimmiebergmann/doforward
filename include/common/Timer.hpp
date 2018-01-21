/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#pragma once

#include <Time.hpp>

namespace dof
{

	/**
	* @breif Timer class.
	*
	*/
	class Timer
	{

	public:

		/**
		* @breif Constructor.
		*
		*/
		Timer();

		/**
		* @breif Start timer.
		*
		*/
		void Start();

		/**
		* @breif Stop timer.
		*
		*/
		void Stop();

		/**
		* @breif Get the time since last stop.
		*
		* @return The current time of the timer.
		*
		*/
		Time GetTime();

		/**
		* @breif Stop and get time.
		*
		* @see Stop
		* @see GetTime
		*
		* @return The current time of the timer.
		*
		*/
		Time GetLapsedTime();

		/**
		* @breif Get system time in microseconds since last startup.
		*
		*/
		static unsigned long long GetSystemTime();

	private:

		unsigned long long m_StartTime;		///< The start time
		unsigned long long m_Time;			///< The the current time.

	};
}