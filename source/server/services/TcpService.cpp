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
#include <network/Poller.hpp>
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
			Network::Socket::Handle listenHandle = m_ListenSocket.GetHandle();
			Network::Socket::Handle	newHandle = 0;

			Network::Poller poller([this](	const std::vector<Network::Socket::Handle> & read,
											const std::vector<Network::Socket::Handle> & write)
			{
				std::cout << "Poller events: Read: " << read.size() << ", Write: " << write.size() << std::endl;

			}, m_Config.MaxConnections, 64, 4);


			poller.Add(listenHandle);

			// Run while serivce is running.
			while (m_Started.Get())
			{
				std::cout << "Listen for new connection." << std::endl;
				// Accept incoming connections.
				if ((newHandle = accept(listenHandle, NULL, NULL)) < 0)
				{
					throw new Exception(Exception::Network, "Failed to accept socket. Error no. " + std::to_string(GetLastError()));
				}

				std::cout << "New connection accepted." << std::endl;

				// Create and add peer.
				Node * pNode = m_pBalancer->GetNext();
				Network::TcpSocket * pNewSocket = new Network::TcpSocket(newHandle, Network::TcpSocket::Peer);
				m_Peers.insert({ newHandle, new TcpPeer(pNewSocket, pNode, nullptr) });
				poller.Add(newHandle, Network::Poller::Read);


				// Poll sockets
				/*std::vector<Network::Socket::Handle> polls;
				poller.Poll(polls, Seconds(10000.0f));

				// Go through polls
				for (auto it = polls.begin(); it != polls.end(); it++)
				{
					Network::Socket::Handle handle = *it;

					// New connection
					if (handle == listenHandle)
					{
						if ((newHandle = accept(listenHandle, NULL, NULL)) < 0)
						{
							throw new Exception(Exception::Network, "Failed to accept socket. Error no. " + std::to_string(GetLastError()));
						}

						

						Node * pNode = m_pBalancer->GetNext();

						// Create and add peer.
						Network::TcpSocket * pNewSocket = new Network::TcpSocket(newHandle, Network::TcpSocket::Peer);
						m_Peers.insert({ newHandle, new TcpPeer(pNewSocket, pNode, nullptr) });
						poller.Add(newHandle);

						std::cout << "Peer connected." << std::endl;
						continue;
					}

					// Peer activity
					// Get peer.
					TcpPeer * pPeer = nullptr;
					auto peerIt = m_Peers.find(handle);
					if (peerIt == m_Peers.end())
					{
						throw new Exception(Exception::Network, "Polled unknown socket.");
					}
					pPeer = peerIt->second;

					// Get new memory pool node.
					MemoryPool<char>::Node * pMemory = m_pMemoryPool->Poll(Seconds(1.0f));
					if (pMemory == nullptr)
					{
						std::cout << "Memory pool timeout." << std::endl;
						continue;
					}

					// Receive data.
					int valRead = 0;
					if ((valRead = recv(handle, pMemory->Get(), pMemory->Size(), 0)) == 0)
					{
						// Disconnection
						std::cout << "Peer disconnected." << std::endl;

						// Erase peer and nove to next poll.
						m_Peers.erase(peerIt);
						delete pPeer;
						poller.Remove(handle);
						continue;
					}

					// Error
					if (valRead < 0)
					{
						throw new Exception(Exception::Network, "Failed to recv on socket. Error no. " + std::to_string(GetLastError()));
					}

					// Print received data.
					pMemory->Get()[valRead] = 0;
					std::cout << "Recv data: " << pMemory->Get() << std::endl;

					m_pMemoryPool->Return(pMemory);
				}*/

			}
			
		});
	}

	void TcpService::Stop()
	{
		m_Started = false;
		m_ListenSocket.Close();
		if (m_pThread)
		{
			m_pThread->join();
			delete m_pThread;
			m_pThread = nullptr;
		}

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