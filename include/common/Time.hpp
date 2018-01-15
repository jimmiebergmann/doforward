/*
* MIT License
*
* Copyright(c) 2017 Jimmie Bergmann
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

namespace dof
{

	/**
	* @breif Time class.
	*
	*/
	class Time
	{

	public:

		friend Time Seconds(const double);
		friend Time Milliseconds(const unsigned long long);
		friend Time Microseconds(const unsigned long long);

		/**
		* @breif default constructor.
		*
		*/
		Time();

		/**
		* @breif Get the time in seconds.
		*
		* @return Time in seconds as a 64 bit floating point value.
		*
		*/
		double AsSeconds() const;

		/**
		* @breif Get the time in milliseconds.
		*
		* @return Time in seconds as a 64 bit unsigned integer value.
		*
		*/
		unsigned long long AsMilliseconds() const;

		/**
		* @breif Get the time in microseconds.
		*
		* @return Time in seconds as a 64 bit unsigned integer value.
		*
		*/
		unsigned long long AsMicroseconds() const;

		/**
		* @breif Value for representing infinite time.
		*
		*/
		static const Time Infinite;

		/**
		* @breif Value for representing zero time.
		*
		*/
		static const Time Zero;

		/**
		* @breif Operators.
		*
		*/
		bool operator ==	(const Time & time) const;
		bool operator !=	(const Time & time) const;
		bool operator >		(const Time & time) const;
		bool operator <		(const Time & time) const;
		bool operator >=	(const Time & time) const;
		bool operator <=	(const Time & time) const;
		Time operator +		(const Time & time) const;
		Time & operator +=	(const Time & time);
		Time operator -		(const Time & time) const;
		Time & operator -=	(const Time & time);
		Time operator *		(const unsigned long long microseconds) const;
		Time operator /		(const unsigned long long microseconds) const;
		Time operator %		(const Time & time) const;

	private:

		/**
		* @breif Constructor.
		*
		*/
		Time(const unsigned long long & microseconds);

		unsigned long long m_Microseconds;	///< Time in nanoseconds.

	};

	/**
	* @breif Value for representing zero time.
	*
	* @param seconds Time in seconds.
	*
	*/
	Time Seconds(const double seconds);

	/**
	* @breif Value for representing zero time.
	*
	* @param milliseconds Time in milliseconds.
	*
	*/
	Time Milliseconds(const unsigned long long milliseconds);

	/**
	* @breif Value for representing zero time.
	*
	* @param microseconds Time in microseconds.
	*
	*/
	Time Microseconds(const unsigned long long microseconds);

}