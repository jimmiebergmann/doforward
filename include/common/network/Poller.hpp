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

#include <Network.hpp>
#include <Time.hpp>
#include <Exception.hpp>
#include <network/Socket.hpp>
#include <initializer_list>
#include <vector>
#include <set>
#include <string>

namespace dof
{

	namespace Network
	{

		/**
		* @breif The poller class is used for polling new read events on sockets.
		*		 Limit of socket count is defined by user, makes it possible to exceed select() limits.
		*
		*/
		class Poller
		{

		public:

			/**
			* @breif Constructor.
			*
			* @param size		Size of socket set. Maximum number of sockets in poller.
			* @param handles	List of sockets to initially add to poller.
			*
			*/
			Poller(const size_t size, const std::initializer_list<Socket::Handle> & handles = {}) :
				m_MaxSize(size),
				m_SetSize(0),
				m_pSet(new pollfd[size])
			{
				for (auto it = handles.begin(); it != handles.end(); it++)
				{
					Add(*it);
				}
				m_SetSize = handles.size();
			}

			/**
			* @breif Destructor.
			*
			*/
			~Poller()
			{
				delete m_pSet;
			}

			/**
			* @breif Poll read events of sockets in poller set.
			*
			* @param polled		Vector of polled sockets.
			* @param timeout	Time until function timeout.
			*
			* @return number of polled sockets. < 0 if error.
			*
			*/
			size_t Poll(std::vector<Socket::Handle> & polled, const Time & timeout = Time::Infinite)
			{
				Compress();

				int finalTimeout = 0;
				if (timeout.AsMilliseconds() > 2147483647ULL)
				{
					finalTimeout = -1;
				}
				else
				{
					finalTimeout = static_cast<int>(timeout.AsMilliseconds());
				}

				int count = WSAPoll(m_pSet, m_SetSize, finalTimeout);
				if (count)
				{
					polled.reserve(count);
					for (size_t i = 0; i < m_SetSize; i++)
					{
						if (m_pSet[i].revents & (POLLIN | POLLHUP))
						{
							polled.push_back(m_pSet[i].fd);
						}
					}
				}

				if (count < 0)
				{
					throw new Exception(Exception::InvalidInput, "Poll failed: Error no. " + std::to_string(WSAGetLastError()));
				}

				return count;
			}

			/**
			* @breif Add socket to poller set.
			*
			*/
			void Add(Socket::Handle handle)
			{
				if (m_SetSize == m_MaxSize)
				{
					throw new Exception(Exception::InvalidInput, "Poll set overflow.");
				}

				for (size_t i = 0; i < m_SetSize; i++)
				{
					if (m_pSet[i].fd == handle)
					{
						return;
					}
				}

				m_pSet[m_SetSize].events = POLLIN;
				m_pSet[m_SetSize].fd = handle;
				m_SetSize++;
			}

			/**
			* @breif Remove socket from poller set.
			*
			*/
			void Remove(Socket::Handle handle)
			{
				for (size_t i = 0; i < m_SetSize; i++)
				{
					if (m_pSet[i].fd == handle)
					{
						m_CompressIndices.insert(i);
					}
				}
			}
			
		private:

			/**
			* @breif Compress poller set, removing sockets from set.
			*
			*/
			void Compress()
			{
				if (m_CompressIndices.size() == 0)
				{
					return;
				}

				if (m_SetSize == 1)
				{
					m_SetSize = 0;
					m_CompressIndices.clear();
					return;
				}

				size_t removed = 0;
				for (auto it = m_CompressIndices.begin(); it != m_CompressIndices.end(); it++)
				{
					size_t index = *it - removed;

					memcpy(m_pSet + index, m_pSet + index + 1, m_SetSize - index - 1);

					m_SetSize--;
					removed++;
				}

				m_CompressIndices.clear();
			}

			size_t				m_MaxSize;			///< Maxmimum size of size.
			size_t				m_SetSize;			///< Current set size.
			pollfd *			m_pSet;				///< Array of poll file descriptors.
			std::set<size_t>	m_CompressIndices;	///< Set containing indices to move left in array.
			

		};

	}

}
