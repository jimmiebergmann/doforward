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

#include <Network/Poller.hpp>
#include <System.hpp>
#include <Exception.hpp>
#include <iostream>

#if defined(DOF_PLATFORM_WINDOWS)
#define DOF_HAVE_SELECT 1
#define DOF_HAVE_POLL 0
#elif defined(DOF_PLATFORM_LINUX)
#define DOF_HAVE_SELECT 0
#define DOF_HAVE_POLL 1
#else
#error Select or Poll is not available.
#endif

static size_t GetMaxWorkerSize(const size_t maxSocketCount, const size_t requestedWorkerSize, const size_t minWorkerCount);
static size_t GetMaxWorkerCount(const size_t maxSocketCount, const size_t requestedWorkerSize, const size_t minWorkerCount);

namespace dof
{

	namespace Network
	{

		Poller::Poller(	const Function & function,
						const size_t maxSocketCount,
						const size_t workerSize,
						const size_t minWorkerCount) :
			m_Function(function),
			m_MaxSocketCount(maxSocketCount),
			m_WorkerSize(GetMaxWorkerSize(maxSocketCount, workerSize, minWorkerCount)),
			m_MaxWorkerCount(GetMaxWorkerCount(maxSocketCount, workerSize, minWorkerCount))
		{
			if (maxSocketCount == 0)
			{
				throw new Exception(Exception::InvalidInput, "Invalid value of maxSocketCount. Expecting > 0.");
			}
			if(m_WorkerSize == 0)
			{
				throw new Exception(Exception::InvalidInput, "Invalid value of workerSize. Expecting > 0.");
			}
		}

		Poller::~Poller()
		{
			m_WorkerMap.Mutex.lock();
			for (auto it = m_WorkerMap.Value.begin(); it != m_WorkerMap.Value.end(); it++)
			{
				delete it->second;
			}
			m_WorkerMap.Mutex.unlock();
		}

		void Poller::Add(const Socket::Handle handle, const unsigned int events)
		{
			SafeGuard sf1(m_WorkerMap);
			SafeGuard sf2(m_SocketAssociationMap);

			// Exit if handle already has been added.
			auto saIt = m_SocketAssociationMap.Value.find(handle);
			if (saIt != m_SocketAssociationMap.Value.end())
			{
				return;
			}

			// Get next worker
			Worker * pWorker = nullptr;
			size_t socketCountInCurrentWorker = 0;
			const size_t socketCount = m_SocketAssociationMap.Value.size();
			const size_t workerCount = m_WorkerMap.Value.size();
			const size_t levelSize = m_WorkerSize / m_MaxWorkerCount;
			const size_t currentWorkerLimit = workerCount * workerCount * levelSize;

			// Create new worker if needed.
			if (workerCount == 0 || socketCount > currentWorkerLimit)
			{
				pWorker = new Worker(m_Function);
			}
			// Get next available worker with least added sockets.
			else
			{
				auto wmIt = m_WorkerMap.Value.begin();
				pWorker = wmIt->second;
				socketCountInCurrentWorker = wmIt->first;
				m_WorkerMap.Value.erase(wmIt);
			}

			// Add worker with new socket count
			socketCountInCurrentWorker++;
			m_WorkerMap.Value.insert({ socketCountInCurrentWorker, pWorker});

			// Add socket to association map
			m_SocketAssociationMap.Value.insert({handle, pWorker});
		
			// Finally we are done by adding handle to worker.
			pWorker->Add(handle, events);
		}

		void Poller::Remove(const Socket::Handle handle)
		{
			SafeGuard sf1(m_WorkerMap);
			SafeGuard sf2(m_SocketAssociationMap);

			// Exit if handle is unknown.
			auto saIt = m_SocketAssociationMap.Value.find(handle);
			if (saIt == m_SocketAssociationMap.Value.end())
			{
				return;
			}

			Worker * pWorker = saIt->second;
			pWorker->Remove(handle);
			m_SocketAssociationMap.Value.erase(handle);
		}

		Poller::Worker::Worker(Function & function) :
			m_Running(true),
			m_pThread(nullptr),
			m_Function(function),
			m_Alerted(false),
			m_AlertSocket(0)
		{
			CreateAlertSocket();

			m_pThread = new std::thread([this]()
			{
				while (m_Running)
				{
#if DOF_HAVE_SELECT
					fd_set readFDs;
					fd_set writeFDs;
					int maxFDs = 0;

					FD_ZERO(&readFDs);
					FD_ZERO(&writeFDs);

					// Add alert socket to read set.
					FD_SET(m_AlertSocket, &readFDs);
					maxFDs = static_cast<int>(m_AlertSocket);

					// Create read / write sets
					m_SetMutex.lock();
					for (auto it = m_ReadSet.begin(); it != m_ReadSet.end(); it++)
					{
						FD_SET(*it, &readFDs);
						if (*it > maxFDs)
						{
							maxFDs = static_cast<int>(*it);
						}
					}
					for (auto it = m_WriteSet.begin(); it != m_WriteSet.end(); it++)
					{
						FD_SET(*it, &writeFDs);
						if (*it > maxFDs)
						{
							maxFDs = static_cast<int>(*it);
						}
					}
					m_SetMutex.unlock();

					// Select
					int count = select(maxFDs, &readFDs, &writeFDs, NULL, NULL);
					if (count == 0 || m_Running == false)
					{
						continue;
					}
					if (FD_ISSET(m_AlertSocket, &readFDs))
					{
						RenewAlertSocket();
						if (count == 1)
						{
							continue;
						}
					}

					// Construct vectors of sockets with new events.
					m_SetMutex.lock();

					std::vector<Socket::Handle> readVec;
					std::vector<Socket::Handle> writeVec;

					for (auto it = m_ReadSet.begin(); it != m_ReadSet.end(); it++)
					{
						if (FD_ISSET(*it, &readFDs))
						{
							readVec.push_back(*it);
						}
					}
					for (auto it = m_WriteSet.begin(); it != m_WriteSet.end(); it++)
					{
						if (FD_ISSET(*it, &writeFDs))
						{
							writeVec.push_back(*it);
						}
					}

					m_SetMutex.unlock();
#endif
#if DOF_HAVE_POLL
#error Poll is not yet implemented.
#endif
					// Call polling function if we got any new events.
					if (readVec.size() || writeVec.size())
					{
						m_Function(readVec, writeVec);
					}

				}
			});
		}


		Poller::Worker::~Worker()
		{
			m_Running = false;
			Alert();

			if (m_pThread)
			{
				m_pThread->join();
				delete m_pThread;
			}

			if (m_Alerted.Get() == false)
			{
				closesocket(m_AlertSocket);
			}
		}

		void Poller::Worker::Add(const Socket::Handle handle, const unsigned int events)
		{
			SafeGuard sf(m_SetMutex);

			if (events & Poller::Read)
			{
				m_ReadSet.insert(handle);
			}
			if (events & Poller::Write)
			{
				m_WriteSet.insert(handle);
			}

			Alert();
		}

		void Poller::Worker::Remove(const Socket::Handle handle)
		{
			SafeGuard sf(m_SetMutex);

			m_ReadSet.erase(handle);
			m_WriteSet.erase(handle);
		}

		void Poller::Worker::Alert()
		{
			SafeGuard sf(m_Alerted);

			if (m_Alerted.Value == true)
			{
				return;
			}

			m_Alerted.Value = true;
			closesocket(m_AlertSocket);
		}

		void Poller::Worker::CreateAlertSocket()
		{
			m_AlertSocket = socket(AF_INET, SOCK_DGRAM, 0);
			if (m_AlertSocket == INVALID_SOCKET)
			{
				throw new Exception(Exception::InvalidPointer, "Failed to create alert socket.");
			}
		}

		void Poller::Worker::RenewAlertSocket()
		{
			SafeGuard alterGuard(m_Alerted);
			if (m_Alerted.Value == true)
			{
				CreateAlertSocket();
				m_Alerted.Value = false;
			}
		}

	}

}

size_t GetMaxWorkerSize(const size_t maxSocketCount, const size_t requestedWorkerSize, const size_t minWorkerCount)
{
	if (maxSocketCount == 0)
	{
		return 0;
	}

	size_t workerSize = requestedWorkerSize;
	if (minWorkerCount > maxSocketCount / requestedWorkerSize)
	{
		workerSize = maxSocketCount / minWorkerCount;
		if (maxSocketCount % minWorkerCount)
		{
			workerSize++;
		}
	}

#ifdef DOF_PLATFORM_WINDOWS
	if (workerSize > FD_SETSIZE)
	{
		workerSize = FD_SETSIZE;
	}
#endif

	return workerSize;
}

size_t GetMaxWorkerCount(const size_t maxSocketCount, const size_t requestedWorkerSize, const size_t minWorkerCount)
{
	const size_t workerSize = GetMaxWorkerSize(maxSocketCount, requestedWorkerSize, minWorkerCount);
	if (workerSize == 0)
	{
		throw new dof::Exception(dof::Exception::InvalidInput, "Invalid value of workerSize. Expecting > 0.");
	}

	size_t workerCount = maxSocketCount / workerSize;
	if(maxSocketCount % workerSize)
	{
		workerCount++;
	}

	return workerCount;
}