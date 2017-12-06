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


/*

YAML documentation: http://www.yaml.org/spec/current.html#id2502325

*/

#pragma once

#include <map>
#include <list>
#include <string>
#include <sstream>
#include <stack>
#include <exception>

namespace dof
{

	/**
	* @breif Namespace containing all YAML classes.
	*
	* @see Value
	* @see Reader
	* @see Writer
	*
	*/
	namespace Yaml
	{

		class Node;
		class Scalar;
		class Sequence;
		class Mapping;

		class ParsingError : public std::exception
		{

		public:

			ParsingError(const std::string & message);

		};

		class InternalError : public std::exception
		{

		public:

			InternalError(const std::string & message);

		};


		/**
		* @breif YAML node class.
		*
		* @see Reader
		* @see Writer
		*
		*/
		class Node
		{

		public:
			
			friend class Scalar;

			/**
			* @breif Enumeration of all possible value types.
			*
			*/
			enum eType
			{
				NullType,
				ScalarType,
				SequenceType,
				MappingType
			};

			/**
			* @breif Default constructor.
			*
			*/
			Node(const eType type = NullType);

			/**
			* @breif Destructor.
			*
			*/
			~Node();

			bool IsNull() const;

			bool IsScalar() const;

			bool IsSequence() const;

			bool IsMapping() const;

			Scalar & AsScalar() const;

			template<class T>
			T Value() const;

			Node & operator = (const std::string & string);
			Node & operator = (const int number);
			Node & operator = (const long long number);
			Node & operator = (const float number);
			Node & operator = (const double number);


			//Node & operator [](const int & index);
			//Node & operator [](const std::string & string);
			

		private:

			/**
			* @breif Copy constructor.
			*
			*/
			Node(const Node & value);

			Node(Scalar * scalar);

			eType	m_Type;
			Node *	m_pChild;
			Node *	m_pParent;
			void *	m_pDataItem;

		};

		/**
		* @breif YAML scalar class.
		*
		*/
		class Scalar
		{

		public:

			friend class Node;

			Scalar();

			Scalar(const Scalar & scalar);

			Scalar(const std::string & string);
			Scalar(const int number);
			Scalar(const long long number);
			Scalar(const float number);
			Scalar(const double number);

			~Scalar();

			Node & GetNode() const;

			template<class T>
			T Value() const;

		private:

			Scalar(Node * node);

			Node *		m_pNode;
			std::string m_Value;

		};



		/**
		* @breif Static declarations.
		*
		*/
		static Node		EmptyNode;
		static Scalar	EmptyScalar;


		/**
		*	Exception implementations.
		*/
		ParsingError::ParsingError(const std::string & message) :
			std::exception(message.c_str())
		{
		}

		InternalError::InternalError(const std::string & message) :
			std::exception(message.c_str())
		{
		}


		/**
		*	Node implementation.
		*/
		Node::Node(const eType type) :
			m_Type(type),
			m_pParent(nullptr),
			m_pChild(nullptr),
			m_pDataItem(nullptr)
		{
			if (m_Type == ScalarType)
			{
				m_pDataItem = new Scalar(this);
			}
		}

		Node::~Node()
		{
		}

		bool Node::IsNull() const
		{
			return m_Type == NullType;
		}

		bool Node::IsScalar() const
		{
			return m_Type == ScalarType;
		}

		bool Node::IsSequence() const
		{
			return m_Type == SequenceType;
		}

		bool Node::IsMapping() const
		{
			return m_Type == MappingType;
		}

		Scalar & Node::AsScalar() const
		{
			if (m_pDataItem == nullptr || m_Type != ScalarType)
			{
				EmptyScalar.m_pNode = nullptr;
				EmptyScalar.m_Value = "";
				return EmptyScalar;
			}
			return *static_cast<Scalar *>(m_pDataItem);
		}

		template<class T>
		T Node::Value() const
		{
			if (m_pDataItem == nullptr || m_Type != ScalarType)
			{
				return static_cast<T>(0);
			}

			Scalar * pScalar = static_cast<Scalar *>(m_pDataItem);

			return pScalar->Value<T>();
		}

		Node & Node::operator = (const std::string & string)
		{
			Scalar * pScalar = nullptr;

			if (m_Type != ScalarType)
			{
				if (m_pDataItem != nullptr)
				{
					delete m_pDataItem;
				}

				m_Type = ScalarType;
				pScalar = new Scalar(this);
				m_pDataItem = pScalar;
			}
			else
			{
				pScalar = static_cast<Scalar *>(m_pDataItem);
			}

			pScalar->m_Value = string;
			return *this;
		}

		Node & Node::operator = (const int number)
		{
			return *this = std::to_string(number);
		}

		Node & Node::operator = (const long long number)
		{
			return *this = std::to_string(number);
		}

		Node & Node::operator = (const float number)
		{
			return *this = std::to_string(number);
		}
			
		Node & Node::operator = (const double number)
		{
			return *this = std::to_string(number);
		}

		Node::Node(const Node & value)
		{
			throw InternalError("Not supporting Node copy yet.");
		}

		Node::Node(Scalar * scalar) :
			m_Type(ScalarType),
			m_pParent(nullptr),
			m_pChild(nullptr),
			m_pDataItem(scalar)
		{
			if (m_pDataItem == nullptr)
			{
				throw InternalError("Scalar is nullptr, passed to node constructor.");
			}
		}
		


		/**
		*	Scalar implementation.
		*/
		Scalar::Scalar() :
			m_pNode(new Node(this)),
			m_Value("")
		{
		}

		Scalar::Scalar(const Scalar & scalar) :
			m_Value(scalar.m_Value)
		{
		}

		Scalar::Scalar(const std::string & string) :
			m_pNode(new Node(this))
		{
			m_Value = string;
		}
		Scalar::Scalar(const int number) :
			m_pNode(new Node(this))
		{
			m_Value = std::to_string(number);
		}

		Scalar::Scalar(const long long number) :
			m_pNode(new Node(this))
		{
			m_Value = std::to_string(number);
		}

		Scalar::Scalar(const float number) :
			m_pNode(new Node(this))
		{
			m_Value = std::to_string(number);
		}

		Scalar::Scalar(const double number) :
			m_pNode(new Node(this))
		{
			m_Value = std::to_string(number);
		}

		Scalar::~Scalar()
		{

		}

		Node & Scalar::GetNode() const
		{
			if (m_pNode == nullptr)
			{
				return EmptyNode;
			}
			return *m_pNode;
		}

		template<class T>
		T Scalar::Value() const
		{
			std::stringstream ss(m_Value);
			T value;
			ss >> value;
			if (ss.fail())
			{
				return static_cast<T>(0);
			}
			return value;
		}

		Scalar::Scalar(Node * node) :
			m_pNode(node),
			m_Value("")
		{
			if (m_pNode == nullptr)
			{
				throw InternalError("Node is nullptr, passed to scalar constructor.");
			}
		}
		






		/**
		* @breif YAML reader class.
		*		 Use Reader or Writer function de/serialization.
		*
		*/
		/*class Reader
		{

		public:

			void ReadFromFile(const std::string & filename, Value & value);

			void ReadFromMemory(const char * data, const int dataSize, Value & value);

			void ReadFromStream(std::stringstream & stream, Value & value);

		private:

			bool FindStart();

			void ParseLine(const std::string & line);

			void ParseList(const std::string & line);

			void ParseObject(const std::string & line);

			std::string TrimSpaces(const std::string & text);

			std::stack<std::pair<int, Value *>> m_ValueStack;
			std::stringstream *					m_pStream;
			Value *								m_pCurrentValue;
			int									m_CurrentLevel;
			int									m_StartLevel;

		};
		*/
	}

}