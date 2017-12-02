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

#include <Group.hpp>
#include <Node.hpp>
#include <Exception.hpp>

namespace dof
{

	// Group class
	Group::Group(Balancer & balancer, const std::string & name) :
		m_Balancer(balancer),
		m_Name(name)
	{

	}

	Balancer & Group::GetBalancer() const
	{
		return m_Balancer;
	}

	const std::string & Group::GetName() const
	{
		return m_Name;
	}

	void Group::GetNodes(std::set<Node *> & nodes)
	{
		SafeGuard sf(m_Nodes);
		nodes.insert(m_Nodes.Value.begin(), m_Nodes.Value.end());
	}

	void Group::Associate(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
		}

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.insert(node);
	}

	void Group::Associate(Group * group)
	{
		if (group == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid group pointer.");
		}

		SafeGuard sf(m_Nodes);
		group->GetNodes(m_Nodes.Value);
	}

	void Group::Detatch(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid node pointer.");
		}

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.erase(node);
	}

	void Group::Detatch(Group * group)
	{
		if (group == nullptr)
		{
			throw Exception(Exception::InvalidPointer, "Invalid group pointer.");
		}

		std::set<Node *> eraseSet;
		group->GetNodes(eraseSet);

		SafeGuard sf(m_Nodes);
		m_Nodes.Value.erase(eraseSet.begin(), eraseSet.end());
	}

}