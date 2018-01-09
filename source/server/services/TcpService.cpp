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

namespace dof
{

	// Service class
	TcpService::TcpService(	Server & server,
							const std::string & name,
							const Network::Address & host,
							const unsigned short port,
							const unsigned short monitorPort,
							const unsigned int sessionTimeout,
							const unsigned int maxConnections) :
		Service(server),
		m_Name(name),
		m_Host(host),
		m_Port(port),
		m_MonitorPort(monitorPort),
		m_SessionTimeout(sessionTimeout),
		m_MaxConnections(maxConnections)
	{
		/*
		Node * pNode = nullptr;
		for (auto it = nodes.begin(); it != nodes.end(); it++)
		{
			pNode = *it;
			if (pNode == nullptr)
			{
				throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
			}

			Associate(*pNode);
		}
		*/
	};


	TcpService::~TcpService()
	{
		/*
		SafeGuard sf(m_Nodes);
		for (auto it = m_Nodes.Value.begin(); it != m_Nodes.Value.end(); it++)
		{
			m_Server.RemoveNode(*(*it), true);
		}
		*/
	}

	void TcpService::Start()
	{

	}

	void TcpService::Stop()
	{

	}
	
	const std::string & TcpService::GetName() const
	{
		return m_Name;
	}

	Network::Protocol::eTransport TcpService::GetTransportProtocol() const
	{
		return Network::Protocol::Tcp;
	}

	Network::Protocol::eApplication TcpService::GetApplicationProtocol() const
	{
		return Network::Protocol::None;
	}

	const Network::Address & TcpService::GetHost() const
	{
		return m_Host;
	}

	unsigned short TcpService::GetPort() const
	{
		return m_Port;
	}

	unsigned short TcpService::GetMonitorPort() const
	{
		return m_MonitorPort;
	}

	unsigned short TcpService::GetSessionTimeout() const
	{
		return m_SessionTimeout;
	}

	unsigned short TcpService::GetMaxConnections() const
	{
		return m_MaxConnections;
	}

	void TcpService::Associate(Node * node)
	{
		/*if (node.m_pService != nullptr)
		{
			return;
		}
		node.m_pService = this;

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.insert(&node);*/
	}

	void TcpService::Detatch(Node * node)
	{
		/*SafeGuard sf(m_Nodes);
		auto it = m_Nodes.Value.find(&node);
		if (it == m_Nodes.Value.end())
		{
			return;
		}

		node.m_pService = nullptr;
		m_Nodes.Value.erase(&node);*/
	}

	void TcpService::GetNodes(std::set<Node *> & nodes)
	{

	}

}