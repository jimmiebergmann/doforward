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
#include <Safe.hpp>
#include <set>
#include <thread>

namespace dof
{

	class Balancer;
	class Node;

	/**
	* @breif The service class contains information about a specific service,
	*		 balanced and routed by the Balancer class.
	*		 Nodes are associated with services, but not managed or destroyed.
	*
	* @see Balancer
	* @see Node
	*
	*/
	class Service
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param balancer			Reference to balancer.
		* @param peerPort			Peer connection port.
		* @param monitorPort		Monitoring port of nodes.
		* @param sessionTimeout		Session time in seconds. 0 if sessioning is disabled.
		* @param maxConnections		Max concurrent connections. Not used if protocol = Udp.
		* @param nodes				Set of nodes associated with this service.
		* @param groups				Set of groups associated with this service.
		
		*
		* @throw dof::Exception if node or group pointer is invalid in parameter sets.
		*
		*/
		Service(Balancer & balancer,
				const std::string & name = "",
				const Network::Protocol::eType protocol = Network::Protocol::Tcp,
				const Network::Address & host = Network::Address::EmptyIpv4,
				const unsigned short port = 0,
				const unsigned short monitorPort = 0,
				const unsigned int sessionTimeout = 0,
				const unsigned int maxConnections = 0,
				const std::set<Node*> & nodes = {}
				);

		/**
		* @breif Destructor
		*
		*/
		~Service();

		/**
		* @breif Start service.
		*		 Service will start to listen for incoming node and peer connections.
		*
		* @throw Exception if startup of service fails.
		*
		*/
		void Start();

		/**
		* @breif Stop service.
		*
		*/
		void Stop();

		/**
		* @breif Get reference to balancer.
		*
		*/
		Balancer & GetBalancer() const;

		/**
		* @breif Get name of service.
		*
		*/
		const std::string & GetName() const;

		/**
		* @breif Get communication protocol.
		*
		*/
		Network::Protocol::eType GetProtocol() const;

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
		* @breif Get communication port of nodes.
		*
		*/
		unsigned short GetMonitorPort() const;

		/**
		* @breif Session timeout. 0 if sessioning is disabled.
		*
		*/
		unsigned short GetSessionTimeout() const;

		/**
		* @breif Get number of max concurrent connections.
		*
		*/
		unsigned short GetMaxConnections() const;

		/**
		* @breif Get nodes associated with this service.
		*
		* @param[in] nodes Reference to set of nodes to poulate with service's nodes.
		*
		*/
		void GetNodes(std::set<Node *> & nodes);

		/**
		* @breif Associate node or group with service.
		*
		* @param node Node to associate.
		*
		*/
		void Associate(Node & node);

		/**
		* @breif Detatch node from service.
		*
		* @param node Node to detatch.
		*
		*/
		void Detatch(Node & node);

		/**
		* @breif Compare operator between services.
		*
		*/
		bool operator == (const Service & service);
		bool operator != (const Service & service);

	private:

		/**
		* @breif Copy constructor.
		*
		*/
		Service(const Service & service);

		Balancer &					m_Balancer;			///< Reference to balancer.
		std::string					m_Name;				///< Name of service.
		Network::Protocol::eType	m_Protocol;			///< Communication protocol.
		Network::Address			m_Host;				///< Host address.
		unsigned short				m_Port;				///< Host port.
		unsigned short				m_MonitorPort;		///< Monitoring port of nodes.
		unsigned int				m_MaxConnections;	///< Max concurrent connections. Not used if protocol = Udp.
		unsigned int				m_SessionTimeout;	///< Session timeout in seconds.
		Safe<std::set<Node *>>		m_Nodes;			///< Set of associated nodes.

	};

}
