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

#include <Safe.hpp>
#include <Semaphore.hpp>
#include <Exception.hpp>
#include <Timer.hpp>
#include <queue>
#include <thread>
#include <algorithm>

namespace dof
{

	/**
	* @breif Memory pool class,
	*		 helping to allocate memory and makes sure new buffers are available as soon as possible.
	*
	*/
	template <typename Type>
	class MemoryPool
	{

	public:

		/**
		* @breif Pool node. Contains the allocation.
		*
		*/
		class Node
		{

		public:

			friend class MemoryPool<Type>;

			/**
			* @breif Get data pointer.
			*
			*/
			Type * Get() const
			{
				return m_pData;
			}

			/**
			* @breif Get data size.
			*
			*/
			unsigned int Size() const
			{
				return m_Size;
			}

		private:

			/**
			* @breif Constructor.
			*
			* @param size	Size of node.
			*
			*/
			Node(const unsigned int size) :
				m_pData(new Type[size]),
				m_Size(size)
			{
			}

			/**
			* @breif Destructor.
			*
			*/
			~Node()
			{
				delete[] m_pData;
			}

			unsigned int	m_Size;		///< Size of data.
			Type *			m_pData;	///< Pointer to data.

		};

		/**
		* @breif Constructor.
		*
		* @param size				Size of each node, of type <typename Type>.
		* @param nodeCount			Number of nodes preallocated.
		* @param maxNodeCount		Maximum number of allocated nodes. Excluded reserved nodes.
		* @param reserveCount		Number of nodes in reservice.
		* @param allocationCount	Number of nodes to allocate at a time.
		*
		* @throw Exception If input is invalid.
		*
		*/
		MemoryPool(	const unsigned int size,
					const unsigned int nodeCount,
					const unsigned int maxNodeCount,
					const unsigned int reserveCount,
					const unsigned int allocationCount = 10) :
			m_Size(size),
			m_NodeCount(nodeCount),
			m_MaxNodeCount(maxNodeCount > nodeCount ? maxNodeCount : nodeCount),
			m_ReserveCount(reserveCount),
			m_AllocationCount(std::min(m_MaxNodeCount - m_NodeCount, allocationCount)),
			m_Running(false),
			m_CurrentNodeCount(nodeCount),
			m_pAllocationThread(nullptr)
		{
			if (size == 0)
			{
				throw new Exception(Exception::InvalidInput, "Invalid size of memory pool. Expected > 0.");
			}
			if (nodeCount == 0)
			{
				throw new Exception(Exception::InvalidInput, "Invalid node count of memory pool. Expected > 0.");
			}

			for (unsigned int i = 0; i < nodeCount; i++)
			{
				m_NodeQueue.Value.push(new Node(size));
			}
			for (unsigned int i = 0; i < reserveCount; i++)
			{
				m_ReserveQueue.Value.push(new Node(size));
			}

			m_Running = true;
			m_pAllocationThread = new std::thread([this]()
			{
				while (1)
				{
					m_AllocationRequest.Wait();

					if (m_Running.Get() == false)
					{
						return;
					}

					{
						SafeGuard sf1(m_NodeQueue);
						SafeGuard sf2(m_ReserveQueue);

						const unsigned int reserveAlloc = std::min<unsigned int>(m_AllocationCount, m_ReserveCount - m_ReserveQueue.Value.size());
						const unsigned int nodeAlloc = std::min<unsigned int>(m_AllocationCount - reserveAlloc, m_NodeCount - m_NodeQueue.Value.size());

						for (unsigned int i = 0; i < reserveAlloc; i++)
						{
							m_ReserveQueue.Value.push(new Node(m_Size));
						}

						for (unsigned int i = 0; i < nodeAlloc; i++)
						{
							m_NodeQueue.Value.push(new Node(m_Size));
						}
					}

					m_AllocationTrigger.NotifyAll();
				}

			});
		}

		/**
		* @breif Destructor.
		*
		*/
		~MemoryPool()
		{
			m_Running = false;
			if (m_pAllocationThread)
			{
				m_AllocationRequest.NotifyOne();
				m_pAllocationThread->join();
				delete m_pAllocationThread;
			}

			while (m_NodeQueue.Value.size())
			{
				delete m_NodeQueue.Value.front();
				m_NodeQueue.Value.pop();
			}
			while (m_ReserveQueue.Value.size())
			{
				delete m_ReserveQueue.Value.front();
				m_ReserveQueue.Value.pop();
			}
		}

		/**
		* @breif Poll next node in pool.
		*
		* @return Pointer to node. Nullptr if timeout is reached.
		*
		*/
		Node * Poll(const Time & timeout = Time::Infinite)
		{
			Time timeoutLeft = timeout;
			Timer timer;

			while (1)
			{
				timer.Start();

				// Return node from main queue if available.
				{
					SafeGuard sf(m_NodeQueue);

					if (m_NodeQueue.Value.size())
					{
						Node * pNode = m_NodeQueue.Value.front();
						m_NodeQueue.Value.pop();
						return pNode;
					}
				}

				// The main queue is empty, check reserve queue and allocate more.
				{
					SafeGuard sf(m_ReserveQueue);

					m_AllocationRequest.NotifyOne();

					if (m_ReserveQueue.Value.size())
					{
						Node * pNode = m_ReserveQueue.Value.front();
						m_ReserveQueue.Value.pop();
						return pNode;
					}
				}

				// Wait for new allocaiton from allocaiton thread.
				if (timeoutLeft == Time::Zero || m_AllocationTrigger.WaitFor(timeoutLeft) == false)
				{
					return nullptr;
				}

				timeoutLeft -= timer.GetLapsedTime();
			}

			// Should never each this.
			return nullptr;
		}

		/**
		* @breif Return node to pool.
		*
		*/
		void Return(Node * node)
		{
			if (node == nullptr)
			{
				throw new Exception(Exception::InvalidInput, "Returning nullptr node to memory pool.");
			}

			{
				SafeGuard sf(m_ReserveQueue);
				if (m_ReserveQueue.Value.size() != m_ReserveCount)
				{
					m_ReserveQueue.Value.push(node);
					return;
				}
			}

			{
				SafeGuard sf(m_NodeQueue);
				if (m_NodeQueue.Value.size() < m_MaxNodeCount)
				{
					m_NodeQueue.Value.push(node);
					return;
				}
				else
				{
					delete node;
					return;
				}
			}
		}

	private:

		typedef std::queue<Node*> NodeQueue;

		const unsigned int m_Size;					///< Size of each node, of type <typename Type>.
		const unsigned int m_NodeCount;				///< Number of nodes preallocated.
		const unsigned int m_MaxNodeCount;			///< Maximum number of allocated nodes.
		const unsigned int m_ReserveCount;			///< Number of nodes in reservice.
		const unsigned int m_AllocationCount;		///< Number of nodes to allocate at a time.

		Safe<bool>			m_Running;				///< Flag indicating the pool is up and running.

		Safe<unsigned int>	m_CurrentNodeCount;		///< Number of allocated nodes in main queue.
		Safe<NodeQueue>		m_NodeQueue;			///< Queue of nodes in main queue.
		Safe<NodeQueue>		m_ReserveQueue;			///< Queue of reserved nodes.

		std::thread *		m_pAllocationThread;	///< Thread used for allocating new nodes.
		Semaphore			m_AllocationRequest;	///< Semaphore for requesting new allocation.
		Semaphore			m_AllocationTrigger;	///< Semaphore triggering completion of new allocation.

	};
}