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

#include <Service.hpp>
#include <set>
#include <thread>

namespace dof
{

	/**
	* @breif The service class contains information about a specific service,
	*		 balanced and routed by the Balancer class.
	*		 Nodes are associated with services, but not managed or destroyed.
	*
	* @see Service
	*
	*/
	class TcpService : public Service
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param server				Reference to server.
		* @param name				Name of service.
		* @param host				Host address.
		* @param port				Host port.
		* @param monitorPort		Monitoring port of nodes.
		* @param sessionTimeout		Session time in seconds. 0 if sessioning is disabled.
		* @param maxConnections		Max concurrent connections. Not used if protocol = Udp.
		*
		* @throw dof::Exception if node or group pointer is invalid in parameter sets.
		*
		*/
		TcpService( Server & server,
					const std::string & name,
					const Network::Address & host,
					const unsigned short port,
					const unsigned short monitorPort,
					const unsigned int sessionTimeout,
					const unsigned int maxConnections
		);

		/**
		* @breif Destructor
		*
		*/
		~TcpService();

		/**
		* @breif Start service.
		*		 Service will start to listen for incoming node and peer connections.
		*
		* @throw Exception if startup of service fails.
		*
		*/
		virtual void Start();

		/**
		* @breif Stop service.
		*
		*/
		virtual void Stop();

		/**
		* @breif Get name of service.
		*
		*/
		virtual const std::string & GetName() const;

		/**
		* @breif Get transport layer protocol.
		*
		*/
		virtual Network::Protocol::eTransport GetTransportProtocol() const;

		/**
		* @breif Get application layer protocol.
		*
		*/
		virtual Network::Protocol::eApplication GetApplicationProtocol() const;

		/**
		* @breif Get host address.
		*
		*/
		virtual const Network::Address & GetHost() const;

		/**
		* @breif Get host port.
		*
		*/
		virtual unsigned short GetPort() const;

		/**
		* @breif Get communication port of nodes.
		*
		*/
		virtual unsigned short GetMonitorPort() const;

		/**
		* @breif Session timeout. 0 if sessioning is disabled.
		*
		*/
		virtual unsigned short GetSessionTimeout() const;

		/**
		* @breif Get number of max concurrent connections.
		*
		*/
		virtual unsigned short GetMaxConnections() const;

		/**
		* @breif Associate node with service.
		*
		* @param node Node to associate.
		*
		*/
		virtual void Associate(Node * node);

		/**
		* @breif Detatch node from service.
		*
		* @param node Node to detatch.
		*
		*/
		virtual void Detatch(Node * node);

		/**
		* @breif Get set of associated nodes.
		*
		* @param node Output set of nodes associated with service.
		*
		*/
		virtual void GetNodes(std::set<Node *> & nodes);


	private:

		/**
		* @breif Copy constructor.
		*
		*/
		TcpService(const TcpService & service);

		std::string					m_Name;				///< Name of service.
		Network::Address			m_Host;				///< Host address.
		unsigned short				m_Port;				///< Host port.
		unsigned short				m_MonitorPort;		///< Monitoring port of nodes.
		unsigned int				m_MaxConnections;	///< Max concurrent connections.
		unsigned int				m_SessionTimeout;	///< Session timeout in seconds.

	};

}
