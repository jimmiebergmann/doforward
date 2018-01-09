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

#pragma once

#include <Network.hpp>
#include <network/Address.hpp>
#include <string>
#include <set>

namespace dof
{

	class Server;
	class Node;

	/**
	* @breif The service class contains information about a specific service,
	*		 balanced and routed by the Balancer class.
	*		 Nodes are associated with services, but not managed or destroyed.
	*
	* @see Server
	* @see Node
	*
	*/
	class Service
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param server Reference to server.
		*
		*/
		Service(Server & server);

		/**
		* @breif Destructor
		*
		*/
		virtual ~Service();
		
		/**
		* @breif Get reference to server.
		*
		*/
		Server & GetServer() const;

		/**
		* @breif Start service.
		*		 Service will start to listen for incoming node and peer connections.
		*
		* @throw Exception if startup of service fails.
		*
		*/
		virtual void Start() = 0;

		/**
		* @breif Stop service.
		*
		*/
		virtual void Stop() = 0;

		/**
		* @breif Get name of service.
		*
		*/
		virtual const std::string & GetName() const = 0;

		/**
		* @breif Get transport layer protocol.
		*
		*/
		virtual Network::Protocol::eTransport GetTransportProtocol() const = 0;

		/**
		* @breif Get application layer protocol.
		*
		*/
		virtual Network::Protocol::eApplication GetApplicationProtocol() const = 0;

		/**
		* @breif Get host address.
		*
		*/
		virtual const Network::Address & GetHost() const = 0;

		/**
		* @breif Get host port.
		*
		*/
		virtual unsigned short GetPort() const = 0;

		/**
		* @breif Get communication port of nodes.
		*
		*/
		virtual unsigned short GetMonitorPort() const = 0;

		/**
		* @breif Session timeout. 0 if sessioning is disabled.
		*
		*/
		virtual unsigned short GetSessionTimeout() const = 0;

		/**
		* @breif Get number of max concurrent connections.
		*
		*/
		virtual unsigned short GetMaxConnections() const = 0;

		/**
		* @breif Associate node with service.
		*
		* @param node Node to associate.
		*
		*/
		virtual void Associate(Node * node) = 0;

		/**
		* @breif Detatch node from service.
		*
		* @param node Node to detatch.
		*
		*/
		virtual void Detatch(Node * node) = 0;

		/**
		* @breif Get set of associated nodes.
		*
		* @param node Output set of nodes associated with service.
		*
		*/
		virtual void GetNodes(std::set<Node *> & nodes) = 0;
		
	private:

		Server &	m_Server;	///< Reference to server.

	};

}
