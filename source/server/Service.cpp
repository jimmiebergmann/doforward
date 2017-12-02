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
#include <Group.hpp>
#include <Node.hpp>
#include <Exception.hpp>

namespace dof
{

	// Service class
	Service::Service(	Balancer & balancer,
						const eProtocol protocol,
						const unsigned short peerPort,
						const unsigned short nodePort,
						const unsigned int maxConnections,
						const unsigned int sessionTimeout,
						const std::set<Node*> & nodes,
						const std::set<Group*> & groups) :
		m_Balancer(balancer),
		m_Protocol(protocol),
		m_PeerPort(peerPort),
		m_NodePort(nodePort),
		m_MaxConnections(maxConnections),
		m_SessionTimeout(sessionTimeout)
	{
		Node * pNode = nullptr;
		for (auto it = nodes.begin(); it != nodes.end(); it++)
		{
			pNode = *it;
			if (pNode == nullptr)
			{
				throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
			}

			m_Nodes.Value.insert(*it);
		}

		Group * pGroup = nullptr;
		for (auto it = groups.begin(); it != groups.end(); it++)
		{
			pGroup = *it;
			if (pGroup == nullptr)
			{
				throw Exception(Exception::InvalidPointer, "Invalid group pointer.");
			}

			pGroup->GetNodes(m_Nodes.Value);
		}

	};

	Service::~Service()
	{

	}

	Balancer & Service::GetBalancer() const
	{
		return m_Balancer;
	}

	Service::eProtocol Service::GetProtocol() const
	{
		return m_Protocol;
	}

	unsigned short Service::GetPeerPort() const
	{
		return m_PeerPort;
	}

	unsigned short Service::GetNodePort() const
	{
		return m_NodePort;
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

	void Service::Associate(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
		}

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.insert(node);
	}

	void Service::Associate(Group * group)
	{
		if (group == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid group pointer.");
		}

		SafeGuard sf(m_Nodes);
		group->GetNodes(m_Nodes.Value);
	}

	void Service::Detatch(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
		}

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.erase(node);
	}

	void Service::Detatch(Group * group)
	{
		if (group == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid group pointer.");
		}

		std::set<Node *> eraseSet;
		group->GetNodes(eraseSet);

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.erase(eraseSet.begin(), eraseSet.end());
	}

}