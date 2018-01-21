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
		m_ListenSocket(0),
		m_pMemoryPool(nullptr),
		m_pPoller(nullptr)
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
		delete m_pPoller;
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

			// Peer data reader poller
			// No write events should be sent here
			m_pPoller = new Network::Poller([this](	const std::vector<Network::Socket::Handle> & read,
													const std::vector<Network::Socket::Handle> & write)
			{
				MemoryPool<char>::Node * pMemory = nullptr;

				// Go through all read sockets
				for (auto it = read.begin(); it != read.end(); it++)
				{
					Network::Socket::Handle handle = *it;
					
					// Get peer
					TcpPeer * pPeer = FindPeer(handle);
					if (pPeer == nullptr)
					{
						std::cout << "Peer has been destoryed, ignore current handle." << std::endl;
						continue;
					}

					// Get new memory pool node.
					if (pMemory == nullptr)
					{
						pMemory = m_pMemoryPool->Poll(Seconds(1.0f));
						if (pMemory == nullptr)
						{
							std::cout << "Memory pool timeout." << std::endl;
							continue;
						}
					}

					// Receive data.
					int valRead = 0;
					if ((valRead = recv(handle, pMemory->Get(), pMemory->Size(), 0)) == 0)
					{
						// Disconnection
						std::cout << "Peer disconnected." << std::endl;
						DestroyPeer(pPeer);
						continue;
					}

					// Error
					if (valRead < 0)
					{
						const DWORD lastError = GetLastError();
						std::cout << "Peer disconnected, unexpectedly, error no. " << lastError << std::endl;
						DestroyPeer(pPeer);
						continue;
					}

					// Print received data.
					pMemory->Get()[valRead] = 0;
					std::cout << "Recv data: " << pMemory->Get() << std::endl;

					m_pMemoryPool->Return(pMemory);
					pMemory = nullptr;
				}

				if (pMemory != nullptr)
				{
					m_pMemoryPool->Return(pMemory);
				}

			}, m_Config.MaxConnections, 64, 4);

			// Run while serivce is running.
			while (m_Started.Get())
			{
				std::cout << "Listen for new connection." << std::endl;
				// Accept incoming connections.
				if ((newHandle = accept(listenHandle, NULL, NULL)) < 0)
				{
					throw new Exception(Exception::Network, "Failed to accept socket. Error no. " + std::to_string(GetLastError()));
				}				

				// Create and add peer.
				if (CreatePeer(newHandle) == nullptr)
				{
					std::cout << "Peer connected, but aborted due to connection limit or no node connected." << std::endl;
					closesocket(newHandle);
					continue;
				}

				std::cout << "Peer connected and accepted." << std::endl;
			}
			
		});
	}

	void TcpService::Stop()
	{
		m_Started = false;
		m_ListenSocket.Close();
		m_ListenSocket = 0;
		if (m_pThread)
		{
			m_pThread->join();
			delete m_pThread;
			m_pThread = nullptr;
		}

		m_Peers.Mutex.lock();
		for (auto it = m_Peers.Value.begin(); it != m_Peers.Value.end();)
		{
			TcpPeer * pPeer = it->second;
			delete pPeer;
		}
		m_Peers.Mutex.unlock();
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

	TcpPeer * TcpService::CreatePeer(const Network::Socket::Handle handle)
	{
		SafeGuard sf(m_Peers);

		if (m_Peers.Value.size() == m_Config.MaxConnections)
		{
			return nullptr;
		}

		Node * pNode = m_pBalancer->GetNext();
		if (pNode == nullptr)
		{
			return nullptr;
		}

		Network::TcpSocket * pNewSocket = new Network::TcpSocket(handle, Network::TcpSocket::Peer);
		TcpPeer * pTcpPeer = new TcpPeer(pNewSocket, pNode, nullptr);
		m_Peers.Value.insert({ handle, pTcpPeer });
		m_pPoller->Add(handle, Network::Poller::Read);

		return pTcpPeer;
	}

	void TcpService::DestroyPeer(const TcpPeer * peer)
	{
		if (peer == nullptr)
		{
			throw new Exception(Exception::InvalidInput, "Invalid input, peer is nullptr.");
		}

		SafeGuard sf(m_Peers);

		const Network::TcpSocket * socket = peer->GetSocket();
		if (socket == nullptr)
		{
			throw new Exception(Exception::InvalidInput, "Invalid peer socket.");
		}

		const Network::Socket::Handle handle = socket->GetHandle();
		auto it = m_Peers.Value.find(handle);
		if (it == m_Peers.Value.end())
		{
			throw new Exception(Exception::InvalidInput, "Unknown handle.");
		}

		// Erase peer
		m_Peers.Value.erase(it);
		delete peer;
		m_pPoller->Remove(handle);
	}

	TcpPeer * TcpService::FindPeer(const Network::Socket::Handle handle)
	{
		SafeGuard sf(m_Peers);
		
		auto it = m_Peers.Value.find(handle);
		if(it == m_Peers.Value.end())
		{
			return nullptr;
		}

		return it->second;
	}

}