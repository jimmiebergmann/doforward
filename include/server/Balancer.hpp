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

namespace dof
{

	class Service;
	class Node;
	class Peer;

	/**
	* @breif The balancer class will pick the most suitable Node,
	*		 for a new connection or new packet.
	*		 Multiple balancing algoritms are available.
	*
	*/
	class Balancer
	{

	public:

		enum eAlgorithm
		{
			RoundRobin,
			ConnectionCount
		};

		/**
		* @breif Constructor.
		*
		*/
		Balancer(Service * service);

		/**
		* @breif Destructor.
		*
		*/
		virtual ~Balancer();

		/**
		* @breif Get pointer to service owning this balancer.
		*
		*/
		Service * GetService() const;

		/**
		* @breif Get algorithm of balancer.
		*
		*/
		virtual eAlgorithm GetAlgoritm() const = 0;

		/**
		* @breif Get next suitable node.
		*
		* @param peer Pointer to peer if available, else nullptr.
		*
		* @return pointer to next node, nullptr if queue is empty.
		*
		*/
		virtual Node * GetNext(Peer * peer = nullptr) = 0;

		/**
		* @breif Associate node with balancer.
		*
		* @throw Exception if node is nullptr.
		*
		*/
		virtual void Associate(Node * node) = 0;

		/**
		* @breif Detatch node from balancer.
		*
		* @throw Exception if node is nullptr.
		*
		*/
		virtual void Detatch(Node * node) = 0;

		/**
		* @breif Detatch all nodes from balancer.
		*
		*/
		virtual void DetatchAll() = 0;

		/**
		* @breif Copy nodes to given balancer.
		*
		* @throw Exception if balancer is nullptr.
		*
		*/
		virtual size_t Copy(Balancer * balancer) = 0;

	private:

		Service * m_pService; ///< Pointer to service.

	};

}
