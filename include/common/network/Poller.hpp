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

#include <network/Socket.hpp>
#include <Safe.hpp>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <atomic>

namespace dof
{

	namespace Network
	{

		/**
		* @breif The poller class is used for polling new read/write events of sockets
		*		 without any limit of socket count. By using multiple threads to select/poll events,
		*		 the procedure of polling is way faster for multicore processors.
		*		 The maximum count of sockets per thread is defined by user, or clamped by system limitations.
		*		 Win32 systems will use select() for polling, linux will use poll().
		*
		*/
		class Poller
		{

		public:

			/**
			* @breif Enumeration of polling events.
			*
			*/
			enum eEvents
			{
				Read = 1,
				Write = 2
			};

			/**
			* @breif Polling function.
			*
			* @param #1 Vector of read events of sockets.
			* @param #2 Vector of write events of sockets.
			*
			*/
			typedef std::function<void(	const std::vector<Socket::Handle> &,
										const std::vector<Socket::Handle> &)> Function;

			/**
			* @breif Constructor.
			*		 Starting threads and polling immidiate after construction.
			*
			* @param function			Pointer to function, executed when new read/write events are available
			* @param maxSocketCount		Maximum number of sockets in poller.
			* @param workerSize			Maximum number of sockets in each worker.
			*							Clamped by system limitations and minWorkerCount if needed.
			* @param minWorkerCount		Minimum worker count when socketsCount = maxSocketCount.
			*							Clamped by maxSocketCount and workerSize.
			*
			*/
			Poller(	const Function & function,
					const size_t maxSocketCount,
					const size_t workerSize,
					const size_t minWorkerCount);

			/**
			* @breif Destructor
			*
			*/
			~Poller();

			/**
			* @breif Add new socket to poller.
			*
			*/
			void Add(const Socket::Handle handle, const unsigned int events = Read|Write);

			/**
			* @breif Remove socket from poller.
			*
			*/
			void Remove(const Socket::Handle handle);

		private:

			/**
			* @breif Worker class.
			*		 The poller is constructed by worker classes.
			*
			*/
			class Worker
			{

			public:

				/**
				* @breif Constructor.
				*
				*/
				Worker(Function & function);

				/**
				* @breif Destructor
				*
				*/
				~Worker();

				/**
				* @breif Add new socket to worker.
				*
				*/
				void Add(const Socket::Handle handle, const unsigned int events);

				/**
				* @breif Remove socket from worker.
				*
				*/
				void Remove(const Socket::Handle handle);

			private:

				/**
				* @breif Alert polling. Will cause the polling function to exit.
				*
				*/
				void Alert();

				/**
				* @breif Create alerting socket.
				*
				*/
				void CreateAlertSocket();

				/**
				* @breif Renew alert socket.
				*
				*/
				void RenewAlertSocket();

				std::atomic<bool>			m_Running;		///< Flag to indicate if thread is running.
				std::thread *				m_pThread;		///< Worker thread.
				Function &					m_Function;		///< Poller function, called when new events are handled.

				std::mutex					m_SetMutex;		///< Mutex for socket sets.
				std::set<Socket::Handle>	m_ReadSet;		///< Set of read sockets.
				std::set<Socket::Handle>	m_WriteSet;		///< Set of write sockets.

				Safe<bool>					m_Alerted;		///< Altert flag for polling.
				Socket::Handle				m_AlertSocket;	///< Socket used for alerting polling.

			};
			
			Safe<std::multimap<size_t, Worker *>>		m_WorkerMap;			///< Map of workers, key = count of sockets in worker.
			Safe<std::map<Socket::Handle, Worker *>>	m_SocketAssociationMap;	///< Map of all sockets and their associated workers.
			Function									m_Function;				///< Function handling polled events.
			const size_t								m_MaxSocketCount;		///< Maximum count of sockets.
			const size_t								m_MaxWorkerCount;		///< Maximum count of workers.
			const size_t								m_WorkerSize;			///< Size of each worker.

		};

	}

}
