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

			Mapping & AsMapping() const;

			template<class T>
			T Value() const;

			void Clear();

			Node & operator = (const std::string & string);
			Node & operator = (const int number);
			Node & operator = (const long long number);
			Node & operator = (const float number);
			Node & operator = (const double number);
			Node & operator = (const Node & node);
			Node & operator = (const Scalar & scalar);
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

			Scalar(const std::string & string);
			Scalar(const int number);
			Scalar(const long long number);
			Scalar(const float number);
			Scalar(const double number);

			~Scalar();

			Node & GetNode() const;

			template<class T>
			T Value() const;

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

			Sequence();

			~Sequence();

			Node & GetNode() const;

			Node & operator [](const int index);

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

			Node & GetNode() const;

			Node & operator [](const std::string & key);

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

			void ReadFromString(const std::string & string, Node & root);

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
			* @throw ParsingError
			*
			* @return String of keyword.
			*
			*/
			std::string FindKeyword(size_t & keyEnd);

			/**
			* @breif Find character not inside quotion marks.
			*
			* @throw ParsingError
			*
			* @return Position of character in current line.
			*
			*/
			size_t FindNotInQuote(char find);

			/**
			* @breif Read next line in stream.
			*
			* @throw ParsingError if reading from stream fails.
			*
			* @return true if successful, false if end of stream.
			*
			*/
			bool Reader::ReadNextLine(std::string & line, size_t & start);

			struct ReaderData
			{
				ReaderData(std::stringstream & stream);

				std::stringstream & Stream;
				size_t Start;
				size_t Offset;
				std::string Line;
				
			};


			ReaderData * m_pData;

			/*bool FindStart();

			void ParseLine(const std::string & line);

			void ParseList(const std::string & line);

			void ParseObject(const std::string & line);

			std::string TrimSpaces(const std::string & text);*/

			/*std::stack<std::pair<int, Value *>> m_ValueStack;
			std::stringstream *					m_pStream;
			Value *								m_pCurrentValue;
			int									m_CurrentLevel;
			int									m_StartLevel;*/

		};
		

		/**
		* @breif Static declarations.
		*
		*/
		static Node		EmptyNode;
		static Scalar	EmptyScalar;
		static Mapping	EmptyMapping;


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
				EmptyScalar.Clear();
				return EmptyScalar;
			}

			return *static_cast<Scalar *>(m_pDataItem);
		}

		Mapping & Node::AsMapping() const
		{
			if (m_pDataItem == nullptr || m_Type != MappingType)
			{
				EmptyMapping.Clear();
				return EmptyMapping;
			}

			return *static_cast<Mapping *>(m_pDataItem);
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

		void Node::Clear()
		{
			/// NEED IMPLEMENTATION OF CLEANING.
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

		Node & Node::operator = (const Node & node)
		{
			if (node.m_Type == ScalarType)
			{
				return *this = node.AsScalar().m_Value;
			}

			/// NEED MORE CODE HERE... MISSING CODE.

			return *this;
		}

		Node & Node::operator = (const Scalar & scalar)
		{
			return *this = scalar.m_Value;
		}

		Node & Node::operator = (const Mapping & mapping)
		{
			return *this;
		}

		Node & Node::operator [](const int index)
		{
			Sequence * pSequence = nullptr;

			if (m_Type == SequenceType && m_pDataItem != nullptr)
			{
				pSequence = static_cast<Sequence *>(m_pDataItem);
			}
			else if (m_Type == NullType)
			{
				m_Type = SequenceType;
				pSequence = new Sequence(this);
				m_pDataItem = pSequence;
			}
			else
			{
				EmptyNode.Clear();
				return EmptyNode;
			}

			return (*pSequence)[index];
		}

		Node & Node::operator [](const std::string & key)
		{
			Mapping * pMapping = nullptr;

			if (m_Type == MappingType && m_pDataItem != nullptr)
			{
				pMapping = static_cast<Mapping *>(m_pDataItem);
			}
			else if (m_Type == NullType)
			{
				m_Type = MappingType;
				pMapping = new Mapping(this);
				m_pDataItem = pMapping;
			}
			else
			{
				EmptyNode.Clear();
				return EmptyNode;
			}

			return (*pMapping)[key];
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

		Node::Node(Sequence * sequence) :
			m_Type(SequenceType),
			m_pParent(nullptr),
			m_pChild(nullptr),
			m_pDataItem(sequence)
		{
			if (m_pDataItem == nullptr)
			{
				throw InternalError("Scalar is nullptr, passed to node constructor.");
			}
		}

		Node::Node(Mapping * mapping) :
			m_Type(MappingType),
			m_pParent(nullptr),
			m_pChild(nullptr),
			m_pDataItem(mapping)
		{
			if (m_pDataItem == nullptr)
			{
				throw InternalError("Mapping is nullptr, passed to node constructor.");
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
				EmptyNode.Clear();
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
		
		void Scalar::Clear()
		{
			/// NEED FIX.
			/// WE NEED TO DISCONNECT ANY PARENT NODES.
			m_pNode = nullptr;
			m_Value = "";
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
		*	Sequence implementation.
		*/
		Sequence::Sequence() :
			m_pNode(new Node(this))
		{

		}

		Sequence::~Sequence()
		{

		}

		Node & Sequence::GetNode() const
		{
			if (m_pNode == nullptr)
			{
				EmptyNode.Clear();
				return EmptyNode;
			}
			return *m_pNode;
		}

		Node & Sequence::operator [](const int index)
		{
			auto it = m_Childs.find(index);
			if (it != m_Childs.end())
			{
				return *it->second;
			}

			Node * pNode = new Node();
			m_Childs.insert({ index, pNode });
			return *pNode;
		}

		void Sequence::Clear()
		{
			/// NEED FIX.
			/// WE NEED TO DISCONNECT ANY PARENT NODES.
			m_pNode = nullptr;
			m_Childs.clear();
		}

		Sequence::Sequence(Node * node) :
			m_pNode(node)
		{
			if (m_pNode == nullptr)
			{
				throw InternalError("Node is nullptr, passed to sequence constructor.");
			}
		}


		/**
		*	Scalar implementation.
		*/
		Mapping::Mapping() :
			m_pNode(new Node(this))
		{
		}

		Mapping::~Mapping()
		{
		}

		Node & Mapping::GetNode() const
		{
			if (m_pNode == nullptr)
			{
				EmptyNode.Clear();
				return EmptyNode;
			}
			return *m_pNode;
		}

		Node & Mapping::operator [](const std::string & key)
		{
			auto it = m_Childs.find(key);
			if (it != m_Childs.end())
			{
				return *it->second;
			}

			Node * pNode = new Node();
			m_Childs.insert({key, pNode});
			return *pNode;
		}

		void Mapping::Clear()
		{
			/// NEED FIX.
			/// WE NEED TO DISCONNECT ANY PARENT NODES.
			m_pNode = nullptr;
			m_Childs.clear();
		}

		Mapping::Mapping(Node * node) :
			m_pNode(node)
		{
			if (m_pNode == nullptr)
			{
				throw InternalError("Node is nullptr, passed to mapping constructor.");
			}
		}


		/**
		*	Reader implementation.
		*/
		Reader::Reader() :
			m_pData(nullptr)
		{
		}

		Reader::~Reader()
		{
			if (m_pData)
			{
				delete m_pData;
			}
		}

		void Reader::ReadFromFile(const std::string & filename, Node & root)
		{
			std::ifstream file(filename, std::ifstream::binary);
			if (file.is_open() == false)
			{
				throw ParsingError("Cannot open file.");
			}

			file.seekg(0, file.end);
			size_t fileSize = file.tellg();
			file.seekg(0, file.beg);

			std::unique_ptr<char []> data(new char[fileSize]);
			file.read(data.get(), fileSize);
			file.close();

			ReadFromMemory(data.get(), fileSize, root);
		}

		void Reader::ReadFromString(const std::string & string, Node & root)
		{
			ReadFromMemory(string.c_str(), string.size(), root);
		}

		void Reader::ReadFromMemory(const char * data, const int dataSize, Node & root)
		{
			std::stringstream ss(std::string(data, dataSize));
			ReadFromStream(ss, root);
		}

		void Reader::ReadFromStream(std::stringstream & stream, Node & root)
		{
			if (m_pData)
			{
				delete m_pData;
			}
			m_pData = new ReaderData(stream);

			try
			{
				Parse(root);
			}
			catch (...)
			{
				delete m_pData;
				m_pData = nullptr;
				throw;
			}

			delete m_pData;
			m_pData = nullptr;
		}

		void Reader::Parse(Node & root)
		{
			root.Clear();

			std::string & line = m_pData->Line;
			size_t & start = m_pData->Start;

			// Get first line, ignore document start: "---".
			if (ReadNextLine(line, start) == false)
			{
				return;
			}
			if (line.size() >= 3 && line.substr(0, 3) == "---")
			{
				if (ReadNextLine(line, start) == false)
				{
					return;
				}
			}

			m_pData->Offset = start;

			// Sequence.
			if (line[0] == '-')
			{
				ParseSequence(root = new Sequence());
				return;
			}

			// Mapping.
			if (isalnum(line[0]))
			{
				ParseMapping(root = new Mapping());
				return;
			}

			throw ParsingError("Root not of type sequence of mapping.");
		}

		bool Reader::ParseSequence(Node & node)
		{
			return false;
		}

		bool Reader::ParseMapping(Node & node)
		{
			// Find keyword
			size_t keyEnd = 0;
			std::string key = "";
			size_t offset = 0;
			std::string value = "";
			
			while (1)
			{
				key = FindKeyword(keyEnd);

				value = m_pData->Line.substr(keyEnd);
				size_t valueStart = value.find_first_not_of(' ');

				// Mapping or sequence value of current mapping.
				if (valueStart == std::string::npos)
				{
					throw InternalError("Sequence/Mapping of map not yet implemented.");
				}

				// Scalar
				node[key] = value.substr(valueStart);

				// Get next line.
				if (ReadNextLine(m_pData->Line, offset) == false)
				{
					return false;
				}

				// Current mapping is done.
				if (offset < m_pData->Offset)
				{
					return true;
				}
			}

			return true;
		}

		std::string Reader::FindKeyword(size_t & keyEnd)
		{
			size_t end = FindNotInQuote(':');
			if (end == std::string::npos)
			{
				throw ParsingError("Failed to find keyword.");
			}

			std::string key = m_pData->Line.substr(0, end);
			keyEnd = key.find_last_not_of(' ');
			if (keyEnd != std::string::npos)
			{
				key = key.substr(0, keyEnd);
			}
			else
			{
				keyEnd = end;
			}



			return key;
		}

		size_t Reader::FindNotInQuote(char find)
		{
			bool inQuote = false;

			for (size_t i = 0; i <  m_pData->Line.size(); i++)
			{
				char cur = m_pData->Line[i];
				char last = 0;
				if (i > 0)
				{
					last = m_pData->Line[i - 1];
				}

				if (cur == '"')
				{
					if (last != '\\')
					{
						inQuote = !inQuote;
					}

					continue;
				}

				if (cur == find)
				{
					if (inQuote == false)
					{
						return i;
					}
				}
			}

			return std::string::npos;
		}

		bool Reader::ReadNextLine(std::string & line, size_t & start)
		{
			while(1)
			{
				std::getline(m_pData->Stream, line);

				if (m_pData->Stream.eof())
				{
					return false;
				}
				if (m_pData->Stream.fail())
				{
					throw ParsingError("Failed to read line from stream.");
				}

				if (line.size() == 0)
				{
					continue;
				}

				if (line.find_first_of('\t') != std::string::npos)
				{
					throw ParsingError("Tabs are not allowed in YAML.");
				}

				size_t startPos = line.find_first_not_of(' ');
				if (startPos == std::string::npos)
				{
					continue;
				}
				start = startPos;

				auto endPos = line.find_last_not_of(' ');
				line = line.substr(startPos, endPos - startPos);

				if (line[0] == '#')
				{
					continue;
				}
				if (line.size() >= 3 && line.substr(0, 3) == "...")
				{
					return false;
				}

				break;
			}

			return true;
		}

		Reader::ReaderData::ReaderData(std::stringstream & stream) :
			Stream(stream),
			Start(0),
			Offset(0),
			Line("")
		{

		}


		
	}

}