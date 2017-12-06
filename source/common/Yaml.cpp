/*#include <Yaml.hpp>
#include <Exception.hpp>
#include <fstream>


namespace dof
{

	namespace Yaml
	{

		// Iteration class of value.
		Value::Iterator::Iterator() :
			m_Type(Null)
		{
		}

		bool Value::Iterator::operator == (const Iterator & iterator)
		{
			if (m_Type == List && iterator.m_Type == List)
			{
				return m_Iterator.List == iterator.m_Iterator.List;
			}

			if (m_Type == Object && iterator.m_Type == Object)
			{
				return m_Iterator.Object == iterator.m_Iterator.Object;
			}

			return false;
		}

		bool Value::Iterator::operator != (const Iterator & iterator)
		{
			return !(*this == iterator);
		}

		Value::Iterator & Value::Iterator::operator ++ ()
		{
			if (m_Type == List)
			{
				(*m_Iterator.List)++;
			}

			if (m_Type == Object)
			{
				(*m_Iterator.Object)++;
			}

			return *this;
		}

		Value::Iterator & Value::Iterator::operator -- ()
		{
			if (m_Type == List)
			{
				(*m_Iterator.List)--;
			}

			if (m_Type == Object)
			{
				(*m_Iterator.Object)--;
			}

			return *this;
		}

		Value & Value::Iterator::operator *()
		{
			if (m_Type == List)
			{
				return *(**m_Iterator.List);
			}

			return *(*m_Iterator.Object)->second;
		}

		Value::Iterator::Iterator(ListIterator & iterator) :
			m_Type(List)
		{
			m_Iterator.List = new ListIterator;
			*m_Iterator.List = iterator;
		}

		Value::Iterator::Iterator(ObjectIterator & iterator) :
			m_Type(Object)
		{
			m_Iterator.Object = new ObjectIterator;
			*m_Iterator.Object = iterator;
		}
	

		// Value class
		Value::Value() :
			m_Type(Null)
		{

		}

		Value::~Value()
		{
			if (m_Type == List)
			{
				delete m_Data.ListData;
			}
			if (m_Type == Object)
			{
				delete m_Data.ObjectData;
			}
			if (m_Type == String)
			{
				delete m_Data.StringData;
			}
		}

		Value::Value(const Value & value)
		{

		}

		Value::eType Value::GetType() const
		{
			return m_Type;
		}


		int Value::GetSize() const
		{
			if (m_Type == String)
			{
				return m_Data.StringData->size();
			}
			if (m_Type == List)
			{
				return m_Data.ListData->size();
			}
			if (m_Type == Object)
			{
				return m_Data.ObjectData->size();
			}

			return 0;
		}

		void Value::PushFront(const Value & value)
		{
			if (m_Type != List)
			{
				return;
			}

			Value * pNewVal = new Value(value);
			m_Data.ListData->push_front(pNewVal);
		}

		void Value::PushBack(const Value & value)
		{
			if (m_Type != List)
			{
				return;
			}

			Value * pNewVal = new Value(value);
			m_Data.ListData->push_back(pNewVal);
		}

		void Value::Insert(const std::string & key, const Value & value)
		{
			if (m_Type != Object)
			{
				if (m_Type != Null)
				{
					throw Exception(Exception::InvalidType, "Yaml value is of type List, cannot insert key values.");
				}
				
				m_Type = Object;
				m_Data.ObjectData = new std::map<std::string, Value *>;
			}
			else
			{
				auto it = m_Data.ObjectData->find(key);
				if (it != m_Data.ObjectData->end())
				{
					(*it->second).CopyValue(value);
					return;
				}
			}

			Value * pNewVal = new Value(value);
			m_Data.ObjectData->insert(std::pair<std::string, Value*>(key, pNewVal));
		}

		Value::Iterator Value::Erase(const std::string & key)
		{
			if (m_Type != Value::Object)
			{
				return Iterator();
			}

			auto it = m_Data.ObjectData->find(key);
			if (it == m_Data.ObjectData->end())
			{
				return Iterator();
			}

			it = m_Data.ObjectData->erase(it);
			return Iterator(it);
		}

		Value::Iterator Value::Erase(Iterator & iterator)
		{
			if (iterator.m_Type == Iterator::List && m_Type == List)
			{
				return Iterator(m_Data.ListData->erase(*(iterator.m_Iterator.List)));
			}

			if (iterator.m_Type == Iterator::Object && m_Type == Object)
			{
				
				return Iterator(m_Data.ObjectData->erase(*(iterator.m_Iterator.Object)));
			}

			return Iterator();
		}

		Value::Iterator Value::Begin() const
		{
			if (m_Type == List)
			{
				return Iterator(m_Data.ListData->begin());
			}
			if (m_Type == Object)
			{
				return Iterator(m_Data.ObjectData->begin());
			}

			return Iterator();
		}

		Value::Iterator Value::End() const
		{
			if (m_Type == List)
			{
				return Iterator(m_Data.ListData->end());
			}
			if (m_Type == Object)
			{
				return Iterator(m_Data.ObjectData->end());
			}

			return Iterator();
		}

		Value & Value::operator = (const Value & value)
		{
			CopyValue(value);
			return *this;
		}

		Value & Value::operator = (const std::string & string)
		{
			ClearData();
			m_Type = String;
			m_Data.StringData = new std::string;
			*m_Data.StringData = string;
			return *this;
		}

		Value & Value::operator = (const int number)
		{
			ClearData();
			m_Type = Integer;
			m_Data.IntegerData = static_cast<long long>(number);
			return *this;
		}

		Value & Value::operator = (const long long number)
		{
			ClearData();
			m_Type = Integer;
			m_Data.IntegerData = number;
			return *this;
		}

		Value & Value::operator = (const double number)
		{
			ClearData();
			m_Type = Float;
			m_Data.FloatData = number;
			return *this;
		}

		Value & Value::operator [] (const std::string & key)
		{
			if (m_Type != Object)
			{
				ClearData();
				m_Type = Object;
				m_Data.ObjectData = new std::map<std::string, Value *>;
			}

			auto it = m_Data.ObjectData->find(key);
			if (it == m_Data.ObjectData->end())
			{
				Value * pValue = new Value();
				m_Data.ObjectData->insert(std::pair<std::string, Value*>(key, pValue));
				return *pValue;
			}

			return (*it->second);
		}

		Value::Value(const eType type)
		{
			throw Exception(Exception::InvalidPointer, "This function should not been accessed.");
		}

		void Value::CopyValue(const Value & value)
		{
			ClearData();

			switch (value.m_Type)
			{
			case Integer:
			{
				m_Type == Integer;
				m_Data.IntegerData = value.m_Data.IntegerData;
			}
			break;
			case Float:
			{
				m_Type == Float;
				m_Data.FloatData = value.m_Data.FloatData;
			}
			break;
			case String:
			{
				m_Type == String;
				m_Data.StringData = new std::string;
				*m_Data.StringData = *value.m_Data.StringData;
			}
			break;
			case List:
			{
				m_Type == List;
				m_Data.ListData = new std::list<Value *>;
				for (auto it = value.m_Data.ListData->begin(); it != value.m_Data.ListData->end(); it++)
				{
					Value * pValue = new Value(*(*it));
					m_Data.ListData->push_back(pValue);
				}
			}
			break;
			case Object:
			{
				m_Type == Object;
				m_Data.ObjectData = new std::map<std::string, Value *>;
				for (auto it = value.m_Data.ObjectData->begin(); it != value.m_Data.ObjectData->end(); it++)
				{
					Value * pValue = new Value(*(it->second));
					m_Data.ObjectData->insert(std::pair<std::string, Value*>(it->first, pValue));
				}
			}
			break;
				default:
					throw Exception(Exception::InvalidType, "Unknown yaml value type.");
					break;
			}
		}

		void Value::ClearData()
		{
			if (m_Type == String)
			{
				delete m_Data.StringData;
			}
			else if (m_Type == List)
			{
				for (auto it = m_Data.ListData->begin(); it != m_Data.ListData->end(); it++)
				{
					delete *it;
				}
				delete m_Data.ListData;
			}
			else if (m_Type == Object)
			{
				for (auto it = m_Data.ObjectData->begin(); it != m_Data.ObjectData->end(); it++)
				{
					delete (*it).second;
				}
				delete m_Data.ObjectData;
			}

			m_Type = Null;
		}



		// Reader class
		void Reader::ReadFromFile(const std::string & filename, Value & value)
		{
			std::ifstream file(filename, std::ifstream::binary);
			if (file.is_open() == false)
			{
				throw Exception(Exception::CannotOpen, "Cannot open file.");
			}

			file.seekg(0, file.end);
			int fileSize = file.tellg();
			file.seekg(0, file.beg);

			char * pData = new char[fileSize];
			file.read(pData, fileSize);
			file.close();

			ReadFromMemory(pData, fileSize, value);
			delete pData;
		}

		void Reader::ReadFromMemory(const char * data, const int dataSize, Value & value)
		{
			std::stringstream ss(std::string(data, dataSize));
			ReadFromStream(ss, value);
		}

		void Reader::ReadFromStream(std::stringstream & stream, Value & value)
		{
			value.ClearData();

			while (m_ValueStack.size())
			{
				m_ValueStack.pop();
			}
			m_pStream = &stream;
			m_pCurrentValue = &value;
			m_CurrentLevel = 0;
			m_StartLevel = 0;
			
			std::string line = "";

			if (FindStart() == false)
			{
				return;
			}
			m_CurrentLevel = m_StartLevel;
			m_ValueStack.push({ m_CurrentLevel, &value });

			// Go through each line in stream.
			while (std::getline(*m_pStream, line))
			{
				if (line.find_first_of('\t') != std::string::npos)
				{
					throw Exception(Exception::ValidationError, "Tabs are not allowed in yaml.");
				}
				int level = line.find_first_not_of(' ');
				if (level == std::string::npos || line[level] == '#')
				{
					continue;
				}
				
				// Elements of same level, do not touch stack.
				if (level == m_CurrentLevel)
				{
					ParseLine(line);
					continue;
				}

				// Level is less, pop stack
				if (level < m_CurrentLevel)
				{
					std::pair<int, Value *> stackItem = {-1, nullptr};

					bool correctLevel = false;
					while (m_ValueStack.size() > 1)
					{
						m_ValueStack.pop();
						stackItem = m_ValueStack.top();
						if (level < stackItem.first)
						{
							continue;
						}

						if (level == stackItem.first)
						{
							correctLevel = true;
						}
						break;
					}
					if (correctLevel == false && stackItem.first != m_StartLevel)
					{
						throw Exception(Exception::ValidationError, "Current line position is less than start position.");
					}
					
					m_CurrentLevel = level;
					m_pCurrentValue = stackItem.second;
					ParseLine(line);
					continue;
				}
				// level is larger, push stack.
				if (level > m_CurrentLevel)
				{
					//Value

					continue;
				}

			}
		}

		bool Reader::FindStart()
		{
			int lastPos = 0;
			int currentPos = 0;
			int level = 0;
			std::string line = "";

			while (std::getline(*m_pStream, line))
			{
				currentPos = m_pStream->tellg();

				if (line.find_first_of('\t') != std::string::npos)
				{
					throw Exception(Exception::ValidationError, "Tabs are not allowed in yaml.");
				}
				level = line.find_first_not_of(' ');
				if (level == std::string::npos || line[level] == '#')
				{
					lastPos = currentPos;
					continue;
				}

				m_pStream->seekg(lastPos, std::stringstream::beg);
				m_StartLevel = level;
				return true;
			}

			return false;
		}

		void Reader::ParseLine(const std::string & line)
		{
			if (m_pCurrentValue->m_Type == Value::Null)
			{
				if (line[m_CurrentLevel] == '-' || line[m_CurrentLevel] == '[')
				{
					m_pCurrentValue->m_Type = Value::List;
					m_pCurrentValue->m_Data.ListData = new std::list<Value *>;
					ParseList(line);
					return;
				}

				m_pCurrentValue->m_Type = Value::Object;
				m_pCurrentValue->m_Data.ObjectData = new std::map<std::string, Value *>;
				ParseObject(line);
				return;
			}

			if (m_pCurrentValue->m_Type == Value::List)
			{
				ParseList(line);
				return;
			}
			if (m_pCurrentValue->m_Type == Value::Object)
			{
				ParseObject(line);
				return;
			}
		}

		void Reader::ParseList(const std::string & line)
		{

		}

		void Reader::ParseObject(const std::string & line)
		{
			int keyEnd = line.find_first_of(':', m_CurrentLevel);
			if (keyEnd == std::string::npos)
			{
				throw Exception(Exception::ValidationError, "Could not find end of key name of object.");
			}

			const std::string key = TrimSpaces(line.substr(m_CurrentLevel, keyEnd - m_CurrentLevel));
			const std::string value = TrimSpaces(line.substr(keyEnd + 1, line.size() - keyEnd));

			if (value.size() == 0)
			{
				throw Exception(Exception::ValidationError, "What to do here? Is this a start of a list?");
			}

			auto numberSearch = value.find_first_not_of("0123456789.");
			if (numberSearch == std::string::npos)
			{
				std::stringstream ss(value);

				// Integer
				numberSearch = value.find_first_of('.');
				if (numberSearch == std::string::npos)
				{
					long long integer = 0LL;
					ss >> integer;
					(*m_pCurrentValue)[key] = integer;
					return;
				}
				// Float
				else
				{
					double floatingPoint = 0.0f;
					ss >> floatingPoint;
					(*m_pCurrentValue)[key] = floatingPoint;
					return;
				}
			}

			// String
			(*m_pCurrentValue)[key] = value;
		}

		std::string Reader::TrimSpaces(const std::string & text)
		{
			std::string finalText = text;

			auto firstNot = finalText.find_first_not_of(' ');
			if (firstNot != std::string::npos && firstNot != 0)
			{
				finalText = finalText.substr(firstNot, text.size() - firstNot);
			}

			auto lastNot = finalText.find_last_not_of(' ');
			if (lastNot != std::string::npos && lastNot != finalText.size() - 1)
			{
				finalText = finalText.substr(0, lastNot + 1);
			}

			return finalText;
		}

	}

}
*/