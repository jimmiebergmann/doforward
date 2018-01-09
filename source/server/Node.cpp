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
#include <Server.hpp>
#include <Exception.hpp>

namespace dof
{

	// Node class
	Node::Node( Server & server,
				const std::string & name,
				const Network::Protocol::eTransport transportProtocol,
				const Network::Protocol::eApplication applicationProtocol,
				const Network::Address & host,
				unsigned short port) :
		m_Server(server),
		m_pService(nullptr),
		m_Name(name),
		m_TransportProtocol(transportProtocol),
		m_ApplicationProtocol(applicationProtocol),
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

	Server & Node::GetServer() const
	{
		return m_Server;
	}

	Service * Node::GetService() const
	{
		return m_pService;
	}

	const std::string & Node::GetName() const
	{
		return m_Name;
	}

	Network::Protocol::eTransport Node::GetTransportProtocol() const
	{
		return m_TransportProtocol;
	}

	Network::Protocol::eApplication Node::GetApplicationProtocol() const
	{
		return m_ApplicationProtocol;
	}

	const Network::Address & Node::GetHost() const
	{
		return m_Host;
	}

	unsigned short Node::GetPort() const
	{
		return m_Port;
	}

}