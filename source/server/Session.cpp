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

#include <Session.hpp>
#include <Safe.hpp>

namespace dof
{

	Session::Session(const Time & timeout) :
		m_Timeout(timeout)
	{
		m_Timer.Start();
	}

	Session::~Session()
	{

	}

	bool Session::IsValid()
	{
		SafeGuard sf(m_Mutex);
		return GetTimeLeft() > Time::Zero;
	}

	void Session::Reset()
	{
		SafeGuard sf(m_Mutex);
		m_Timer.Start();
	}

	const Time & Session::GetTimeout()
	{
		SafeGuard sf(m_Mutex);
		return m_Timeout;
	}

	Time Session::GetTimeLeft()
	{
		SafeGuard sf(m_Mutex);

		Time lapsed = m_Timer.GetLapsedTime();
		return m_Timeout - lapsed;
	}

}