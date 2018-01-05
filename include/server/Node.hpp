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
#include <string>
#include <thread>

namespace dof
{

	class Balancer;
	class Service;

	/**
	* @breif 
	*
	*/
	class Node
	{

	public:

		friend class Service;

		/**
		* @breif Constructor.
		*
		* @param address		Ip address of node.
		* @param port			Port used for node to connect to server.
		*
		*/
		Node(Balancer & balancer,
			const std::string & name = "",
			const Network::Protocol::eType protocol = Network::Protocol::Tcp,
			const Network::Address & host = 0,
			unsigned short port = 0);

		/**
		* @breif Destructor.
		*
		*/
		~Node();

		/**
		* @breif Get reference to balancer.
		*
		*/
		Balancer & GetBalancer() const;

		/**
		* @breif Get associated service.
		*
		* @return Reference to service, Balance::InvalidService if node is not yet associated.
		*
		*/
		Service & GetService() const;

		/**
		* @breif Get name of node.
		*
		*/
		const std::string & GetName() const;

		/**
		* @breif Get protocol of node.
		*
		*/
		Network::Protocol::eType GetProtocol() const;

		/**
		* @breif Get host address of node.
		*
		*/
		const Network::Address & GetHost() const;

		/**
		* @breif Get host port of node.
		*
		*/
		unsigned short GetPort() const;

		/**
		* @breif Compare operator between nodes.
		*
		*/
		bool operator == (const Node & node);
		bool operator != (const Node & node);

	private:

		/**
		* @breif Copy constructor.
		*
		*/
		Node(const Node & node);

		Balancer &					m_Balancer;	///< Reference to balancer.
		Service *					m_pService;	///< Pointer to associated service.
		std::string					m_Name;		///< Name of node.
		Network::Protocol::eType	m_Protocol;	///< Communication protocol of node.
		Network::Address			m_Host;		///< Host adress of node.
		unsigned short				m_Port;		///< Port used for node to connect to server.


	};

}
