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



#include <Yaml.hpp>

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

		/**
		* @breif Static declarations.
		*
		*/
		static Node		EmptyNode;
		static Scalar	EmptyScalar;
		static Sequence EmptySequence;
		static Mapping	EmptyMapping;


		/**
		*	Exception implementations.
		*/
		ParsingError::ParsingError(const std::string & message) :
			Exception(Exception::Yaml, message)
		{
		}

		InternalError::InternalError(const std::string & message) :
			Exception(Exception::Yaml, message)
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

		Sequence & Node::AsSequence() const
		{
			if (m_pDataItem == nullptr || m_Type != SequenceType)
			{
				EmptySequence.Clear();
				return EmptySequence;
			}

			return *static_cast<Sequence *>(m_pDataItem);
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


		Node & Node::Clear()
		{
			if (m_Type == ScalarType)
			{
				Scalar * pScalar = static_cast<Scalar*>(m_pDataItem);
				delete pScalar;
			}
			else if (m_Type == SequenceType)
			{
				///< REMOVE ALL CHILDS.

				Sequence * pSequence = static_cast<Sequence*>(m_pDataItem);
				delete pSequence;
			}
			else if (m_Type == MappingType)
			{
				///< REMOVE ALL CHILDS.

				Mapping * pMapping = static_cast<Mapping*>(m_pDataItem);
				delete pMapping;
			}
			else
			{
				if (m_pDataItem != nullptr)
				{
					throw InternalError("Cannot clear NullType Node.");
				}
				return *this;
			}

			m_pDataItem = nullptr;
			m_Type = NullType;
			return *this;
		}

		Scalar & Node::ClearAsScalar()
		{
			Clear();
			m_Type = ScalarType;
			Scalar * pScalar = new Scalar(this);
			m_pDataItem = static_cast<void*>(pScalar);
			return *pScalar;
		}

		Sequence & Node::ClearAsSequence()
		{
			Clear();
			m_Type = SequenceType;
			Sequence * pSequence = new Sequence(this);
			m_pDataItem = static_cast<void*>(pSequence);
			return *pSequence;
		}

		Mapping & Node::ClearAsMapping()
		{
			Clear();
			m_Type = MappingType;
			Mapping * pMapping = new Mapping(this);
			m_pDataItem = static_cast<void*>(pMapping);
			return *pMapping;
		}

		size_t Node::Size() const
		{

			if (m_pDataItem == nullptr)
			{
				return 0;
			}

			if (m_Type == SequenceType)
			{
				return static_cast<Sequence *>(m_pDataItem)->Size();
			}
			else if (m_Type == MappingType)
			{
				return static_cast<Mapping *>(m_pDataItem)->Size();
			}

			return 0;
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
				return *this = node.AsScalar();
			}

			/// NEED MORE CODE HERE... MISSING CODE.

			return *this;
		}

		Node & Node::operator = (const Scalar & scalar)
		{
			return *this = scalar.m_Value;
		}

		Node & Node::operator = (const Sequence & sequence)
		{
			throw InternalError("Not implemented yet!");
			return *this;
		}

		Node & Node::operator = (const Mapping & mapping)
		{
			Mapping * pMapping = nullptr;

			if (m_Type != MappingType)
			{
				if (m_pDataItem != nullptr)
				{
					delete m_pDataItem;
				}

				m_Type = MappingType;
				pMapping = new Mapping(this);
				m_pDataItem = pMapping;
			}
			else
			{
				m_pDataItem = static_cast<Mapping *>(m_pDataItem);
			}

			///< SHOULD NOT RUN CORRECTYL. 
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

		Scalar::Scalar(const char * characters) :
			m_pNode(new Node(this))
		{
			m_Value = characters;
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

		Node & Scalar::AsNode() const
		{
			if (m_pNode == nullptr)
			{
				EmptyNode.Clear();
				return EmptyNode;
			}
			return *m_pNode;
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

		Node & Sequence::AsNode() const
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

		Node & Sequence::Append()
		{
			// Find next index.
			int index = 0;

			auto it = m_Childs.rbegin();
			if (it == m_Childs.rend())
			{
				index = 0;
			}
			else
			{
				index = it->first + 1;
			}

			Node * pNode = new Node();
			m_Childs.insert({ index, pNode });
			return *pNode;
		}

		size_t Sequence::Size() const
		{
			return m_Childs.size();
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

		Node & Mapping::AsNode() const
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

		size_t Mapping::Size() const
		{
			return m_Childs.size();
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
			size_t fileSize = static_cast<size_t>(file.tellg());
			file.seekg(0, file.beg);

			std::unique_ptr<char []> data(new char[fileSize]);
			file.read(data.get(), fileSize);
			file.close();

			ReadFromMemory(data.get(), fileSize, root);
		}

		void Reader::ReadFromMemory(const std::string & string, Node & root)
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
			size_t & startOffset = m_pData->StartOffset;

			// Get first line, ignore document start: "---".
			if (ReadNextLine(line, startOffset) == false)
			{
				return;
			}
			if (line.size() >= 3 && line.substr(0, 3) == "---")
			{
				if (ReadNextLine(line, startOffset) == false)
				{
					return;
				}
			}
			m_pData->CurrentOffset = startOffset;


			Node::eType rootType = Node::NullType;

			// Sequence.
			if (line[0] == '-')
			{
				rootType = Node::SequenceType;
				root.ClearAsSequence();
			}
			// Mapping.
			else if (isalnum(line[0]))
			{
				rootType = Node::MappingType;
				root.ClearAsMapping();
			}
			else
			{
				throw ParsingError("Root not of type sequence of mapping.");
			}

			// Keep on reading data until root is clear.
			while (1)
			{
				bool ret = false;

				// Sequence
				if (rootType == Node::SequenceType)
				{
					if(ParseSequence(root) == false)
					{
						return;
					}
				}
				// Mapping.
				else
				{
					if (ParseMapping(root) == false)
					{
						return;
					}
				}
			}
		}

		bool Reader::ParseSequence(Node & node)
		{
			if (node.m_Type != Node::SequenceType)
			{
				throw InternalError("Node not of sequence type.");
			}

			Sequence & sequence = node.AsSequence();

			const size_t seqLevel = m_pData->CurrentOffset;
			size_t offset = 0;

			Node::eType itemType = Node::NullType;
			Node * pCurrentNode = nullptr;

			// Loop sequence data.
			while (1)
			{
				// New item
				if (m_pData->Line[0] == '-')
				{
					// No item data on this line, perhaps on the next one.
					size_t itemStart = m_pData->Line.find_first_not_of(' ', 1);
					if (itemStart == std::string::npos)
					{
						// No item info on this line, get next one.
						if (ReadNextLine(m_pData->Line, offset) == false)
						{
							return false;
						}
						m_pData->CurrentOffset = offset;

						if (offset < seqLevel)
						{
							throw ParsingError("Incorrect Sequence offset. Line: " + std::to_string(m_pData->LineNo) + ".");
						}

						// Sequence of sequence
						if (m_pData->Line[0] == '-')
						{
							Node & newNode = sequence.Append().ClearAsSequence().AsNode();
							newNode.m_pParent = &node;
							node.m_pChild = &newNode;
							if (ParseSequence(newNode) == true)
							{
								continue;
							}
							return false;
						}

						// Mapping
						Node & newNode = sequence.Append().ClearAsMapping().AsNode();
						newNode.m_pParent = &node;
						node.m_pChild = &newNode;

						if (ParseMapping(newNode) == true)
						{
							if (m_pData->CurrentOffset < seqLevel)
							{
								return true;
							}
							continue;
						}

						return false;
					}
					else
					{
						m_pData->Line = m_pData->Line.substr(itemStart);
						m_pData->CurrentOffset += itemStart;
					}

					// Scalar value
					if (m_pData->Line.find_first_of(':') == std::string::npos)
					{
						Scalar & scalar = sequence.Append().ClearAsScalar();
						scalar = m_pData->Line;

						if (ReadNextLine(m_pData->Line, offset) == false)
						{
							return false;
						}
						m_pData->CurrentOffset = offset;

						if (offset < seqLevel)
						{
							return true;
						}

						// Except new item, after scalar and same level.
						if (m_pData->Line[0] != '-')
						{
							throw ParsingError("Excepting Sequence of next line: " + std::to_string(m_pData->LineNo) + ".");
						}

						continue;
					}

					// Mapping
					Node & newNode = sequence.Append().ClearAsMapping().AsNode();
					newNode.m_pParent = &node;
					node.m_pChild = &newNode;

					if (ParseMapping(newNode) == true)
					{
						if (m_pData->CurrentOffset < seqLevel)
						{
							return true;
						}
						continue;
					}

					return false;
				}

			}

			return false;
		}

		bool Reader::ParseMapping(Node & node)
		{
			if (node.m_Type != Node::MappingType)
			{
				throw InternalError("Node not of mapping type.");
			}

			const size_t mapLevel = m_pData->CurrentOffset;
			size_t valueStart = 0;
			std::string key = "";
			size_t offset = 0;
			std::string value = "";

			while (1)
			{
				key = FindKeyword(valueStart);

				// Mapping or sequence value of current mapping.
				if (valueStart == std::string::npos)
				{
					// Get next line.
					if (ReadNextLine(m_pData->Line, offset) == false)
					{
						throw ParsingError("Excepting Sequence/Mapping of next line: " + std::to_string(m_pData->LineNo) + ".");
					}
					if (offset <= m_pData->CurrentOffset)
					{
						throw ParsingError("Incorrect offset of next line: " + std::to_string(m_pData->LineNo) + ".");
					}
					m_pData->CurrentOffset = offset;

					// Sequence.
					if (m_pData->Line[0] == '-')
					{
						Node & newNode = node[key].ClearAsSequence().AsNode();
						newNode.m_pParent = &node;
						node.m_pChild = &newNode;

						if (ParseSequence(newNode) == true)
						{
							continue;
						}
						return false;
					}

					// Mapping.
					if (isalnum(m_pData->Line[0]))
					{
						Node & newNode = node[key].ClearAsMapping().AsNode();
						newNode.m_pParent = &node;
						node.m_pChild = &newNode;

						if (ParseMapping(newNode) == true)
						{
							if (m_pData->CurrentOffset < mapLevel)
							{
								return true;
							}
							continue;
						}

						return false;
					}

					throw ParsingError("Excepting Sequence/Mapping of next line: " + std::to_string(m_pData->LineNo) + ".");
				}

				// Scalar
				node[key] = m_pData->Line.substr(valueStart);
				node[key].m_pParent = &node;

				// Get next line.
				if (ReadNextLine(m_pData->Line, offset) == false)
				{
					return false;
				}

				if (offset > m_pData->CurrentOffset)
				{
					throw InternalError("Incorrect offset of next line: " + std::to_string(m_pData->LineNo) + ".");
				}

				// Current mapping is done.
				if (offset < m_pData->CurrentOffset)
				{
					m_pData->CurrentOffset = offset;
					return true;
				}

				m_pData->CurrentOffset = offset;

				// Continue...
			}

			return true;
		}

		std::string Reader::FindKeyword(size_t & valueStart)
		{
			size_t end = m_pData->Line.find_first_of(':');
			if (end == std::string::npos)
			{
				throw ParsingError("Failed to find keyword. Line: " + std::to_string(m_pData->LineNo) + ".");
			}
			std::string key = m_pData->Line.substr(0, end);

			// No value of key.
			valueStart = key.size() + 1;
			if (valueStart >= m_pData->Line.size())
			{
				valueStart = std::string::npos;
				return key;
			}

			valueStart = m_pData->Line.find_first_not_of(' ', valueStart);
			return key;
		}


		bool Reader::ReadNextLine(std::string & line, size_t & offset)
		{
			while(1)
			{
				std::getline(m_pData->Stream, line);
				if (line.size())
				{
					if (line[line.size() - 1] == '\r')
					{
						line.resize(line.size() - 1);
					}
				}

				m_pData->LineNo++;

				if (m_pData->Stream.eof())
				{
					return false;
				}
				if (m_pData->Stream.fail())
				{
					throw ParsingError("Failed to read line from stream. Line: " + std::to_string(m_pData->LineNo) + ".");
				}

				if (line.size() == 0)
				{
					continue;
				}

				if (line.find_first_of('\t') != std::string::npos)
				{
					throw ParsingError("Tabs are not allowed in YAML. Line: " + std::to_string(m_pData->LineNo) + ".");
				}

				size_t startPos = line.find_first_not_of(' ');
				if (startPos == std::string::npos)
				{
					continue;
				}
				offset = startPos;

				auto endPos = line.find_last_not_of(' ');
				line = line.substr(startPos, endPos - startPos + 1);

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
			StartOffset(0),
			CurrentOffset(0),
			Line(""),
			LineNo(0)
		{

		}


		
	}

}