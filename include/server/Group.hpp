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
#include <set>
#include <string>

namespace dof
{

	class Balancer;
	class Node;

	/**
	* @breif Groups are collections of nodes, making it easier to
	*		 associate nodes to multiple services.
	*		 Nodes are collected in groups, but not managed or destroyed.
	*
	*/
	class Group
	{

	public:

		/**
		* @breif Constructor.
		*
		* @param balancer	Reference to balancer.
		* @param name		Name of group.
		*
		*/
		Group(Balancer & balancer, const std::string & name);

		/**
		* @breif Get reference to balancer.
		*
		*/
		Balancer & GetBalancer() const;

		/**
		* @breif Get name of group.
		*
		*/
		const std::string & GetName() const;

		/**
		* @breif Get nodes of group.
		*
		* @param[in] nodes Reference to set of nodes to poulate with group's nodes.
		*
		*/
		void GetNodes(std::set<Node *> & nodes);

		/**
		* @breif Associate node or group's nodes with group.
		*
		* @param node	Pointer of node to associate.
		* @param group	Pointer of group to associate group's nodes with.
		*
		* @throw dof::Exception if node or group pointer is invalid.
		*
		*/
		void Associate(Node * node);
		void Associate(Group * group);

		/**
		* @breif Detatch node or groups's nodes from group.
		*
		* @param node		Pointer of node to detatch.
		* @param group		Pointer of group to detatch nodes from.
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
		Group(const Group & group);

		Balancer &				m_Balancer; ///< Reference to balancer.
		std::string				m_Name;		///< Name of group.
		Safe<std::set<Node *>>	m_Nodes;	///< Set of associated nodes.

	};

}
