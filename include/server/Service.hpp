/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
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
#include <Balancer.hpp>
#include <Time.hpp>
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
		* @breif Service configuration structure.
		*
		*/
		struct Config
		{

			/**
			* @breif Default constructor.
			*
			*/
			Config();

			/**
			* @breif Service buffer configuration structure.
			*
			*/
			struct Buffer
			{
				/**
				* @breif Default constructor.
				*
				*/
				Buffer();

				unsigned int		Size;					///< Buffer size of each pool node.
				unsigned int		PoolCount;				///< Number of pool nodes preallocated.
				unsigned int		PoolMaxCount;			///< Maximum number of allocated pool nodes.
				unsigned int		PoolReserveCount;		///< Number of reserve pool nodes.
				unsigned int		PoolAllocationCount;	///< Number of nodes allocated at a time.
			};

			std::string 			Name;					///< Name of service.
			Network::Address		Host;					///< Host address of service.
			unsigned short			Port;					///< Name port of service.
			Buffer					BufferInfo;				///< Information about buffering.
			Balancer::eAlgorithm	BalancerAlgorithm;		///< Balancing algorithm used for selecting nodes.
			Time 					SessionTimeout;			///< Timeout of session. Zero if not using sessions.
			unsigned int			MaxConnections;			///< Maximum number of connections. Zero if no limit.

		};

		/**
		* @breif Constructor.
		*
		* @param server Reference to server.
		*
		*/
		Service(Server & server, const Config & config);

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
		* @breif Get configuration structure.
		*
		*/
		const Config & GetConfig() const;

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
		const std::string & GetName() const;

		/**
		* @breif Get host address.
		*
		*/
		const Network::Address & GetHost() const;

		/**
		* @breif Get host port.
		*
		*/
		unsigned short GetPort() const;

		/**
		* @breif Session timeout. Zero time if sessioning is disabled.
		*
		*/
		const Time & GetSessionTimeout() const;

		/**
		* @breif Get number of max concurrent connections.
		*
		*/
		unsigned short GetMaxConnections() const;

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
		
	protected:

		Server &	m_Server;	///< Reference to server.
		Config		m_Config;	///< Configuration structure.

	};

}
