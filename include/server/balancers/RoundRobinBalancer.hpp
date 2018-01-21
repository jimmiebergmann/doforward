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

#include <Balancer.hpp>
#include <Safe.hpp>
#include <list>
#include <set>

namespace dof
{

	/**
	* @breif Balancer using round robin algorithm.
	*		 First in last out.
	*
	* @see Balancer
	*
	*/
	class RoundRobinBalancer : public Balancer
	{

	public:

		/**
		* @breif Constructor.
		*
		*/
		RoundRobinBalancer(Service * service);

		/**
		* @breif Destructor.
		*
		*/
		~RoundRobinBalancer();

		/**
		* @breif Get algorithm of balancer.
		*
		*/
		virtual eAlgorithm GetAlgoritm() const;

		/**
		* @breif Get next suitable node.
		*
		* @param peer Pointer to peer if available, else nullptr.
		*
		* @return pointer to next node, nullptr if queue is empty.
		*
		*/
		virtual Node * GetNext(Peer * peer = nullptr);

		/**
		* @breif Associate node with balancer.
		*
		* @throw Exception if node is nullptr.
		*
		*/
		virtual void Associate(Node * node);

		/**
		* @breif Detatch node from balancer.
		*
		* @throw Exception if node is nullptr.
		*
		*/
		virtual void Detatch(Node * node);

		/**
		* @breif Detatch all nodes from balancer.
		*
		*/
		virtual void DetatchAll();

		/**
		* @breif Copy nodes to given balancer.
		*
		* @throw Exception if balancer is nullptr.
		*
		*/
		virtual size_t Copy(Balancer * balancer);

	private:

		Safe<std::list<Node *>> m_NodeList; ///< Queue list of nodes.
		Safe<std::set<Node *>>	m_NodeSet;	///< Set of all nodes.

	};

}
