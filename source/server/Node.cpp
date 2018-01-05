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

#include <Node.hpp>
#include <Balancer.hpp>
#include <Exception.hpp>

namespace dof
{

	// Node class
	Node::Node(	Balancer & balancer,
				const std::string & name,
				const Network::Protocol::eType protocol,
				const Network::Address & host,
				unsigned short port) :
		m_Balancer(balancer),
		m_pService(nullptr),
		m_Name(name),
		m_Protocol(protocol),
		m_Host(host),
		m_Port(port)
	{
	}

	/**
	* @breif Destructor.
	*
	*/
	Node::~Node()
	{
	}

	/**
	* @breif Get reference to balancer.
	*
	*/
	Balancer & Node::GetBalancer() const
	{
		return m_Balancer;
	}

	Service & Node::GetService() const
	{
		if (m_pService == nullptr)
		{
			return const_cast<Service&>(Balancer::InvalidService);
		}

		return *m_pService;
	}

	const std::string & Node::GetName() const
	{
		return m_Name;
	}

	Network::Protocol::eType Node::GetProtocol() const
	{
		return m_Protocol;
	}

	const Network::Address & Node::GetHost() const
	{
		return m_Host;
	}

	unsigned short Node::GetPort() const
	{
		return m_Port;
	}

	bool Node::operator == (const Node & node)
	{
		if (this == &node)
		{
			return true;
		}

		return false;
	}

	bool Node::operator != (const Node & node)
	{
		if (this != &node)
		{
			return true;
		}

		return false;
	}

}