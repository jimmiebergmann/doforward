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

#include <Service.hpp>
#include <Balancer.hpp>
#include <Node.hpp>
#include <Exception.hpp>

namespace dof
{

	// Service class
	Service::Service(	Balancer & balancer,
						const std::string & name,
						const Network::Protocol::eType protocol,
						const Network::Address & host,
						const unsigned short port,
						const unsigned short monitorPort,
						const unsigned int sessionTimeout,
						const unsigned int maxConnections,
						const std::set<Node*> & nodes) :
		m_Balancer(balancer),
		m_Name(name),
		m_Protocol(protocol),
		m_Host(host),
		m_Port(port),
		m_MonitorPort(monitorPort),
		m_SessionTimeout(sessionTimeout),
		m_MaxConnections(maxConnections)
	{
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
	};

	Service::~Service()
	{
		SafeGuard sf(m_Nodes);
		for (auto it = m_Nodes.Value.begin(); it != m_Nodes.Value.end(); it++)
		{
			m_Balancer.RemoveNode(*(*it), true);
		}
	}

	void Service::Start()
	{

	}

	void Service::Stop()
	{

	}

	Balancer & Service::GetBalancer() const
	{
		return m_Balancer;
	}

	const std::string & Service::GetName() const
	{
		return m_Name;
	}

	Network::Protocol::eType Service::GetProtocol() const
	{
		return m_Protocol;
	}

	const Network::Address & Service::GetHost() const
	{
		return m_Host;
	}

	unsigned short Service::GetPort() const
	{
		return m_Port;
	}

	unsigned short Service::GetMonitorPort() const
	{
		return m_MonitorPort;
	}

	unsigned short Service::GetMaxConnections() const
	{
		return m_MaxConnections;
	}

	unsigned short Service::GetSessionTimeout() const
	{
		return m_SessionTimeout;
	}

	void Service::GetNodes(std::set<Node *> & nodes)
	{
		SafeGuard sf(m_Nodes);
		nodes.insert(m_Nodes.Value.begin(), m_Nodes.Value.end());
	}

	void Service::Associate(Node & node)
	{
		if (node.m_pService != nullptr)
		{
			return;
		}
		node.m_pService = this;

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.insert(&node);
	}

	void Service::Detatch(Node & node)
	{
		SafeGuard sf(m_Nodes);
		auto it = m_Nodes.Value.find(&node);
		if (it == m_Nodes.Value.end())
		{
			return;
		}

		node.m_pService = nullptr;
		m_Nodes.Value.erase(&node);
	}

	bool Service::operator == (const Service & service)
	{
		if (this == &service)
		{
			return true;
		}

		return false;
	}

	bool Service::operator != (const Service & service)
	{
		if (this != &service)
		{
			return true;
		}

		return false;
	}

}