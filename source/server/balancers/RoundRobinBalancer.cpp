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

#include <balancers/RoundRobinBalancer.hpp>
#include <Exception.hpp>

namespace dof
{

	// Service class
	RoundRobinBalancer::RoundRobinBalancer(Service * service) :
		Balancer(service)
	{
	};

	RoundRobinBalancer::~RoundRobinBalancer()
	{
	}

	Balancer::eAlgorithm RoundRobinBalancer::GetAlgoritm() const
	{
		return RoundRobin;
	}

	Node * RoundRobinBalancer::GetNext(Peer * peer)
	{
		SafeGuard sf(m_NodeList);

		if (m_NodeList.Value.size())
		{
			Node * pNode = m_NodeList.Value.front();
			m_NodeList.Value.pop_front();
			m_NodeList.Value.push_back(pNode);
			return pNode;
		}

		return nullptr;
	}

	void RoundRobinBalancer::Associate(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}

		SafeGuard sf1(m_NodeList);
		SafeGuard sf2(m_NodeSet);

		if (m_NodeSet.Value.find(node) != m_NodeSet.Value.end())
		{
			return;
		}

		m_NodeList.Value.push_back(node);
		m_NodeSet.Value.insert(node);
	}

	void RoundRobinBalancer::Detatch(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}

		SafeGuard sf1(m_NodeList);
		SafeGuard sf2(m_NodeSet);

		auto setIt = m_NodeSet.Value.find(node);
		if (setIt == m_NodeSet.Value.end())
		{
			return;
		}

		m_NodeSet.Value.erase(setIt);
		for (auto it = m_NodeList.Value.begin(); it != m_NodeList.Value.end(); it++)
		{
			if (*it == node)
			{
				m_NodeList.Value.erase(it);
				return;
			}
		}
	}

	void RoundRobinBalancer::DetatchAll()
	{
		SafeGuard sf1(m_NodeList);
		SafeGuard sf2(m_NodeSet);

		m_NodeList.Value.clear();
		m_NodeSet.Value.clear();
	}

	size_t RoundRobinBalancer::Copy(Balancer * balancer)
	{
		if (balancer == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Balancer pointer is nullptr.");
		}

		SafeGuard sf(m_NodeSet);
		size_t count = m_NodeSet.Value.size();

		balancer->DetatchAll();
		for (auto it = m_NodeSet.Value.begin(); it != m_NodeSet.Value.end(); it++)
		{
			balancer->Associate(*it);
		}

		return count;
	}

}