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

#include <balancers/ConnectionCountBalancer.hpp>
#include <Exception.hpp>

namespace dof
{

	// Service class
	ConnectionCountBalancer::ConnectionCountBalancer(Service * service) :
		Balancer(service)
	{
	};

	ConnectionCountBalancer::~ConnectionCountBalancer()
	{
	}

	Balancer::eAlgorithm ConnectionCountBalancer::GetAlgoritm() const
	{
		return ConnectionCount;
	}

	Node * ConnectionCountBalancer::GetNext(Peer * peer)
	{
		SafeGuard sf(m_NodeMap);
/*/
		auto it = m_NodeMap.Value.begin();
		if (it == m_NodeMap.Value.end())
		{
			return nullptr;
		}

		Node * pNode = it->second;

		if (peer)
		{
			unsigned int count = it->first + 1;
			m_NodeMap.Value.erase(it);
			m_NodeMap.Value.insert({count, pNode});
		}
		else
		{
			pNode = 
		}

		return pNode;*/

		return nullptr;
	}

	void ConnectionCountBalancer::Associate(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}

		SafeGuard sf1(m_NodeMap);
		SafeGuard sf2(m_NodeSet);
		
		if (m_NodeSet.Value.find(node) != m_NodeSet.Value.end())
		{
			return;
		}

		m_NodeMap.Value.insert({ 0, node });
		m_NodeSet.Value.insert(node);
	}

	void ConnectionCountBalancer::Detatch(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}
		
		SafeGuard sf1(m_NodeMap);
		SafeGuard sf2(m_NodeSet);

		auto setIt = m_NodeSet.Value.find(node);
		if (setIt == m_NodeSet.Value.end())
		{
			return;
		}

		m_NodeSet.Value.erase(setIt);
		for (auto it = m_NodeMap.Value.begin(); it != m_NodeMap.Value.end(); it++)
		{
			if (it->second == node)
			{
				m_NodeMap.Value.erase(it);
				return;
			}
		}
	}

	void ConnectionCountBalancer::DetatchAll()
	{
		SafeGuard sf1(m_NodeMap);
		SafeGuard sf2(m_NodeSet);

		m_NodeMap.Value.clear();
		m_NodeSet.Value.clear();
	}

	size_t ConnectionCountBalancer::Copy(Balancer * balancer)
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