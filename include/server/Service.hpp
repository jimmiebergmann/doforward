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

#include <string>
#include <Safe.hpp>
#include <set>
#include <thread>

namespace dof
{

	class Balancer;
	class Group;
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
		* @breif Enumeration of supported communication protocols.
		*
		*/
		enum eProtocol
		{
			Tcp,
			/*Udp,
			Http,
			Https*/
		};

		/**
		* @breif Constructor.
		*
		* @param balancer			Reference to balancer.
		* @param peerPort			Peer connection port.
		* @param nodePort			Node connection port.
		* @param sessionTimeout		Session time in seconds. 0 if sessioning is disabled.
		* @param maxConnections		Max concurrent connections. Not used if protocol = Udp.
		* @param nodes				Set of nodes associated with this service.
		* @param groups				Set of groups associated with this service.
		
		*
		* @throw dof::Exception if node or group pointer is invalid in parameter sets.
		*
		*/
		Service(Balancer & balancer,
				const eProtocol protocol,
				const unsigned short peerPort,
				const unsigned short nodePort,
				const unsigned int sessionTimeout,
				const unsigned int maxConnections,
				const std::set<Node*> & nodes = {},
				const std::set<Group*> & groups = {}
				);

		/**
		* @breif Destructor
		*
		*/
		~Service();

		/**
		* @breif Get reference to balancer.
		*
		*/
		Balancer & GetBalancer() const;

		/**
		* @breif Get communication protocol.
		*
		*/
		eProtocol GetProtocol() const;

		/**
		* @breif Get peer connection port.
		*
		*/
		unsigned short GetPeerPort() const;

		/**
		* @breif Get communication port of nodes.
		*
		*/
		unsigned short GetNodePort() const;

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
		* @param node	Pointer of node to associate.
		* @param group	Pointer of group to associate.
		*
		* @throw dof::Exception if node or group pointer is invalid.
		*
		*/
		void Associate(Node * node);
		void Associate(Group * group);

		/**
		* @breif Detatch node from service.
		*
		* @param node		Pointer of node to detatch.
		* @param group		Pointer of group to detatch.
		*					All group nodes are detached from service.
		*
		* @throw dof::Exception if node or group pointer is invalid.
		*
		*/
		void Detatch(Node * node);
		void Detatch(Group * group);

	private:

		/**
		* @breif Copy constructor.
		*
		*/
		Service(const Service & service);

		Balancer &				m_Balancer;			///< Reference to balancer.
		eProtocol				m_Protocol;			///< Communication protocol.
		unsigned short			m_PeerPort;			///< Peer connection port.
		unsigned short			m_NodePort;			///< Communication port of nodes.
		unsigned int			m_MaxConnections;	///< Max concurrent connections. Not used if protocol = Udp.
		unsigned int			m_SessionTimeout;	///< Session timeout in seconds.
		Safe<std::set<Node *>>	m_Nodes;			///< Set of associated nodes.
		

	};

}
