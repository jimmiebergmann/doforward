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

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <memory>
#include <Exception.hpp>


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

		class ParsingError : public Exception
		{

		public:

			ParsingError(const std::string & message);

		};

		class InternalError : public Exception
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
		
			friend class Reader;
			friend class Scalar;
			friend class Sequence;
			friend class Mapping;

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

			Sequence & AsSequence() const;

			Mapping & AsMapping() const;

			Node & Clear();

			Scalar & ClearAsScalar();

			Sequence & ClearAsSequence();

			Mapping & ClearAsMapping();


			template<class T>
			T Value() const
			{
				if (m_pDataItem == nullptr || m_Type != ScalarType)
				{
					T tempValue = T();
					return tempValue;
				}
				Scalar * pScalar = static_cast<Scalar *>(m_pDataItem);
				return pScalar->Value<T>();
			}

			template<class T>
			T Value(const T & defaultValue) const
			{
				if (m_pDataItem == nullptr || m_Type != ScalarType)
				{
					return defaultValue;
				}
				Scalar * pScalar = static_cast<Scalar *>(m_pDataItem);
				return pScalar->Value<T>();
			}

			size_t Size() const;


			Node & operator = (const std::string & string);
			Node & operator = (const int number);
			Node & operator = (const long long number);
			Node & operator = (const float number);
			Node & operator = (const double number);
			Node & operator = (const Node & node);
			Node & operator = (const Scalar & scalar);
			Node & operator = (const Sequence & sequence);
			Node & operator = (const Mapping & mapping);
			Node & operator [](const int index);
			Node & operator [](const std::string & key);

		private:

			/**
			* @breif Copy constructor.
			*
			*/
			Node(const Node & value);

			Node(Scalar * scalar);
			Node(Sequence * sequence);
			Node(Mapping * mapping);

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

			Scalar(const char * characters);
			Scalar(const std::string & string);
			Scalar(const int number);
			Scalar(const long long number);
			Scalar(const float number);
			Scalar(const double number);

			~Scalar();

			Node & AsNode() const;


			template<class T>
			T Value() const
			{
				std::stringstream ss(m_Value);
				T value;
				ss >> value;
				if (ss.fail())
				{
					value = T();
				}
				return value;
			}

			template<>
			std::string Value<std::string>() const
			{
				return m_Value;
			}

			void Clear();

		private:

			Scalar(Node * node);

			Node *		m_pNode;
			std::string m_Value;

		};


		/**
		* @breif YAML sequence class.
		*
		*/
		class Sequence
		{

		public:

			friend class Node;
			friend class Reader;

			Sequence();

			~Sequence();

			Node & AsNode() const;

			Node & operator [](const int index);

			Node & Append();

			size_t Size() const;

			void Clear();

		private:

			Sequence(Node * node);

			Node *					m_pNode;
			std::map<int, Node *>	m_Childs;

		};


		/**
		* @breif YAML mapping class.
		*
		*/
		class Mapping
		{

		public:

			friend class Node;

			Mapping();

			~Mapping();

			Node & AsNode() const;

			Node & operator [](const std::string & key);

			size_t Size() const;

			void Clear();

		private:

			Mapping(Node * node);

			Node *							m_pNode;
			std::map<std::string, Node *>	m_Childs;

		};


		/**
		* @breif YAML reader class.
		*		 Use Reader or Writer function de/serialization.
		*
		*/
		class Reader
		{

		public:

			Reader();

			~Reader();

			void ReadFromFile(const std::string & filename, Node & root);

			void ReadFromMemory(const std::string & string, Node & root);
			void ReadFromMemory(const char * data, const int dataSize, Node & root);

			void ReadFromStream(std::stringstream & stream, Node & root);

		private:

			/**
			* @breif Parse stream.
			*
			* @throw ParsingError
			*
			*/
			void Parse(Node & root);

			/**
			* @breif Parse current line as sequence.
			*
			* @throw ParsingError
			*
			* @return true if successful, false if end of stream.
			*
			*/
			bool ParseSequence(Node & node);

			/**
			* @breif Parse current line as mapping.
			*
			* @throw ParsingError
			*
			* @return true if successful, false if end of stream.
			*
			*/
			bool ParseMapping(Node & node);

			/**
			* @breif Get key map of current line.
			*
			* @param valueStart[out] Start position of value.
			*
			* @throw ParsingError
			*
			* @return String of keyword.
			*
			*/
			std::string FindKeyword(size_t & valueStart);

			/**
			* @breif Read next line in stream.
			*
			* @param line[out]		Next line returned, cleaned from initial spaces.
			* @param offset[out]	Line data offset.
			*
			* @throw ParsingError if reading from stream fails.
			*
			* @return true if successful, false if end of stream(no line data).
			*
			*/
			bool ReadNextLine(std::string & line, size_t & offset);

			/**
			* @breif Temporary data for parsing.
			*
			*/
			struct ReaderData
			{
				ReaderData(std::stringstream & stream);

				std::stringstream & Stream;
				size_t StartOffset;
				size_t CurrentOffset;
				std::string Line;
				size_t LineNo;
				
			};

			ReaderData * m_pData; ///< Temporary data for partsing.

		};
		
	}

}