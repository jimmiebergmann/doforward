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
#include <Safe.hpp>
#include <MemoryPool.hpp>
#include <network/TcpSocket.hpp>
#include <network/Poller.hpp>
#include <Balancer.hpp>
#include <set>
#include <map>
#include <thread>

namespace dof
{

	/**
	* @breif Forward declarations.
	*
	*/
	class TcpPeer;
	class Balancer;

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
		* @param server		Reference to server.
		* @param config		Reference to service configurations.
		*
		*/
		TcpService( Server & server, const Config & config);

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
		* @breif Create and add new peer by socket handle.
		*
		* @return Pointer to new peer. nullptr if not created due to exceeded limit.
		*
		*/
		TcpPeer * CreatePeer(const Network::Socket::Handle handle);

		/**
		* @breif Destory and remove peer.
		*
		*/
		void DestroyPeer(const TcpPeer * peer);

		/**
		* @breif Check if peer exists.
		*
		* @return Pointer to found peer. nullptr if not found.
		*
		*/
		TcpPeer * FindPeer(const Network::Socket::Handle handle);

		/**
		* @breif Copy constructor.
		*
		*/
		TcpService(const TcpService & service);
		
		typedef std::map<Network::Socket::Handle, TcpPeer *> PeerMap;

		Safe<bool>					m_Started;			///< Flag indicating the servicer is started.
		std::thread *				m_pThread;			///< Main thread.
		Balancer *					m_pBalancer;		///< Balancer.
		MemoryPool<char> *			m_pMemoryPool;		///< Memory pool for incoming data.
		Network::TcpSocket			m_ListenSocket;		///< Socket for connection listening.
		Safe<PeerMap>				m_Peers;			///< Map of conneced peers, socket handle as key.
		Network::Poller *			m_pPoller;			///< Socket poller.

	};

}
