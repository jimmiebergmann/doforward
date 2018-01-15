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

#include <Timer.hpp>
#include <ctime>
#include <WindowsHeaders.hpp>
//#include <sys/time.h>

namespace dof
{

	Timer::Timer() :
		m_StartTime( 0 ),
		m_Time( 0 )
	{
	}

	void Timer::Start()
	{
		m_StartTime = GetSystemTime();
	}

	void Timer::Stop( )
	{
		m_Time = GetSystemTime() - m_StartTime;
	}

	Time Timer::GetTime()
	{
		return Microseconds(m_Time);
	}

	Time Timer::GetLapsedTime()
	{
		Stop();
		return Microseconds(m_Time);
	}

	unsigned long long Timer::GetSystemTime()
	{
		long long counter = 0;
		long long frequency = 0;

		QueryPerformanceCounter( (LARGE_INTEGER*)&counter );
		QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency );

		double timeFloat = (static_cast<double>(counter) * 1000000.0f ) / static_cast<double>(frequency);
		return static_cast<unsigned long long>(timeFloat);

		// Linux implementation.
		/*
			timeval Time;
			gettimeofday( &Time, 0 );

			return	static_cast<long lon>(Time.tv_sec) * 1000000 + static_cast<long lon>(Time.tv_usec);
		*/
	}

}

