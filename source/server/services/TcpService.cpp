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

#include <services/TcpService.hpp>
#include <network/TcpSocket.hpp>
#include <peers/TcpPeer.hpp>
#include <balancers/ConnectionCountBalancer.hpp>
#include <balancers/RoundRobinBalancer.hpp>
#include <Exception.hpp>
#include <iostream>

namespace dof
{

	// Service class
	TcpService::TcpService(	Server & server, const Config & config) :
		Service(server, config),
		m_Started(false),
		m_pThread(nullptr),
		m_pBalancer(nullptr),
		m_pMemoryPool(nullptr)
	{
		// Create balancer.
		if (config.BalancerAlgorithm == Balancer::RoundRobin)
		{
			m_pBalancer = new RoundRobinBalancer(this);
		}
		else if (config.BalancerAlgorithm == Balancer::ConnectionCount)
		{
			m_pBalancer = new ConnectionCountBalancer(this);
		}
		else
		{
			throw new Exception(Exception::InvalidInput, "Unknown balancing algoritm.");
		}

		// Create memory pool
		m_pMemoryPool = new MemoryPool<char>(
			m_Config.BufferInfo.Size,
			m_Config.BufferInfo.PoolCount,
			m_Config.BufferInfo.PoolMaxCount,
			m_Config.BufferInfo.PoolReserveCount,
			m_Config.BufferInfo.PoolAllocationCount);
	};


	TcpService::~TcpService()
	{
		Stop();
		delete m_pBalancer;
		delete m_pMemoryPool;
	}

	void TcpService::Start()
	{
		// Start to listen.
		m_ListenSocket.Listen(m_Config.Port);

		m_Started = true;
		m_pThread = new std::thread([this]()
		{
			fd_set		socketSet;
			int			maxFD = 0;
			int			currentFD = 0;
			const int	listenSocket = static_cast<int>(m_ListenSocket.GetHandle());
			int			newSocket = 0;
			struct timeval timeout = { 1, 0 };

			while (m_Started.Get())
			{
				/// SHOULD BE OPTIMIZED AND NOT CALCULATED EVERY LOOP?

				// Create set of this loop
				FD_ZERO(&socketSet);
				FD_SET(listenSocket, &socketSet);
				maxFD = listenSocket;
				for (auto it = m_Peers.begin(); it != m_Peers.end(); it++)
				{
					currentFD = static_cast<int>(it->first);
					FD_SET(currentFD, &socketSet);
					if (currentFD > maxFD)
					{
						maxFD = currentFD;
					}
				}

				// Select from socket set.
				int activity = select(maxFD + 1, &socketSet, NULL, NULL, &timeout);
				if (activity == 0) // Timeout.
				{
					
					continue;
				}
				if (activity < 0) // Error.
				{
					throw new Exception(Exception::Network, "Failed to select socket. Error no. " + std::to_string(GetLastError()));
				}

				// Check for new connection.
				if (FD_ISSET(listenSocket, &socketSet))
				{
					if ((newSocket = accept(listenSocket, NULL, NULL)) < 0)
					{
						throw new Exception(Exception::Network, "Failed to accept socket. Error no. " + std::to_string(GetLastError()));
					}					
				}

				// Check activity on connections.
				for (auto it = m_Peers.begin(); it != m_Peers.end();)
				{
					// Check if we got any activity on current peer.
					currentFD = static_cast<int>(it->first);
					if (!FD_ISSET(currentFD, &socketSet))
					{
						++it;
						continue;
					}

					TcpPeer * pPeer = it->second;

					// Pool pool node.
					MemoryPool<char>::Node * pNode = m_pMemoryPool->Poll(Seconds(1.0f));
					if (pNode == nullptr)
					{
						std::cout << "Memory pool timeout." << std::endl;
						continue;
					}

					// Receive data.
					int valRead = 0;
					if ((valRead = recv(currentFD, pNode->Get(), pNode->Size(), 0)) == 0)
					{
						// Disconnection
						std::cout << "Peer disconnected." << std::endl;

						// Erase peer and nove to next iterator.
						it = m_Peers.erase(it);
						delete pPeer;
						continue;
					}

					// Error
					if (valRead < 0)
					{
						throw new Exception(Exception::Network, "Failed to recv on socket. Error no. " + std::to_string(GetLastError()));
					}

					// Print received data.
					pNode->Get()[valRead] = 0;
					std::cout << "Recv data: " << pNode->Get() << std::endl;

					m_pMemoryPool->Return(pNode);

					// Move to next iterator.
					++it;
				}

				// Add connection socket and create peer.
				if (newSocket != 0)
				{
					const Network::Socket::Handle socketHandle = static_cast<Network::Socket::Handle>(newSocket);
					newSocket = 0;
					
					// Select node from balancer.
					Node * pNode = m_pBalancer->GetNext();

					// Create and add peer.
					Network::TcpSocket * pNewSocket = new Network::TcpSocket(socketHandle, Network::TcpSocket::Peer);
					TcpPeer * pNewPeer = new TcpPeer(pNewSocket, pNode, nullptr);
					m_Peers.insert({ socketHandle, pNewPeer });
					
					std::cout << "Peer connected." << std::endl;
				}

			}

		});
	}

	void TcpService::Stop()
	{
		m_Started = false;

		if (m_pThread)
		{
			m_pThread->join();
			delete m_pThread;
			m_pThread = nullptr;
		}

		m_ListenSocket.Close();

		for (auto it = m_Peers.begin(); it != m_Peers.end();)
		{
			TcpPeer * pPeer = it->second;
			delete pPeer;
		}
	}
	
	Network::Protocol::eTransport TcpService::GetTransportProtocol() const
	{
		return Network::Protocol::Tcp;
	}

	Network::Protocol::eApplication TcpService::GetApplicationProtocol() const
	{
		return Network::Protocol::None;
	}

	void TcpService::Associate(Node * node)
	{
		m_pBalancer->Associate(node);
	}

	void TcpService::Detatch(Node * node)
	{
		m_pBalancer->Detatch(node);
	}

	void TcpService::GetNodes(std::set<Node *> & nodes)
	{
		throw new Exception(Exception::InvalidInput, "Function not yet implemented: TcpService::GetNodes");
	}

}