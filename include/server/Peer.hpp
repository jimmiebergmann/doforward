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

#include <Safe.hpp>
#include <Network.hpp>

namespace dof
{

	class Session;
	class Node;

	/**
	* @breif Peer(client) communicating with service.
	*
	*/
	class Peer
	{

	public:

		/**
		* @breif Constructor.
		*
		*/
		Peer(Node * node, Session * session);

		/**
		* @breif Destructor.
		*
		*/
		~Peer();

		/**
		* @breif Get type of peer.
		*
		*/
		virtual Network::Protocol::eTransport GetType() const = 0;

		/**
		* @breif Get pointer to session.
		*
		* @return pointer to session, nullptr if not using session.
		*
		*/
		Session * GetSession();

		/**
		* @breif Get pointer to current node.
		*
		*/
		Node * GetNode();

		/**
		* @breif Set current node of peer.
		*
		*/
		void SetNode(Node * node);

	private:

		Safe<Node *>	m_pNode;	///< Pointer node.
		Session *		m_pSession;	///< Pointer to session.

	};

}
