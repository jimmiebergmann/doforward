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

#include <Timer.hpp>
#include <mutex>

namespace dof
{

	class Node;

	/**
	* @breif The session class is used for handling connection sessions,
	*		 or making sure the same node is used when reconnecting.
	*
	*/
	class Session
	{

	public:


		/**
		* @breif Constructor.
		*
		*/
		Session(const Time & timeout);

		/**
		* @breif Destructor.
		*
		*/
		~Session();

		/**
		* @breif Check if session is valid.
		*
		*/
		bool IsValid();

		/**
		* @breif Reset session timer.
		*
		*/
		void Reset();

		/**
		* @breif Get timeout.
		*
		*/
		const Time & GetTimeout();

		/**
		* @breif Get time left
		*
		*/
		Time GetTimeLeft();

	private:

		Time		m_Timeout;	///< Timeout.
		Timer		m_Timer;	///< Timeout timer.
		std::mutex	m_Mutex;	///< Mutex.

	};

}
