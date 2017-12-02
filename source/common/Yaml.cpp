#include <Yaml.hpp>
#include <Exception.hpp>

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
					(*it->second).CopyData(value);
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
			CopyData(value);
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

		void Value::CopyData(const Value & value)
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

	}

}
