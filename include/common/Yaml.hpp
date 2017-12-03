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

#include <map>
#include <list>
#include <string>
#include <sstream>
#include <stack>

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
		* @breif YAML value class.
		*		 Use Reader or Writer function de/serialization.
		*
		* @see Reader
		* @see Writer
		*
		*/
		class Value
		{

		public:

			class Iterator
			{

			public:

				friend class Value;

				enum eType
				{
					Null,
					List,
					Object
				};

				Iterator();

				bool operator == (const Iterator & iterator);

				bool operator != (const Iterator & iterator);

				Iterator & operator ++ ();

				Iterator & operator -- ();

				Value & operator * ();
				
			private:

				typedef std::list<Value *>::iterator ListIterator;
				typedef std::map<std::string, Value *>::iterator ObjectIterator;

				Iterator(ListIterator & iterator);
				Iterator(ObjectIterator & iterator);

				union IteratorData
				{
					ListIterator *		List;
					ObjectIterator *	Object;
				};

				eType			m_Type;
				IteratorData	m_Iterator;

			};

			friend class Iterator;
			friend class Reader;
			friend class Writer;

			/**
			* @breif Enumeration of all possible value types.
			*
			*/
			enum eType
			{
				Null,
				String,
				Integer,
				Float,
				List,
				Object
			};

			/**
			* @breif Default constructor.
			*
			*/
			Value();

			/**
			* @breif Destructor.
			*
			*/
			~Value();

			/**
			* @breif Copy constructor.
			*
			*/
			Value(const Value & value);

			/**
			* @breif Get type of value.
			*
			*/
			eType GetType() const;

			/**
			* @breif Get type of value.
			*
			* @return 0 if type = Number.
			*		  Length of string if type = String.
			*		  Size of list if type = List.
			*		  Count of values if type = Object.
			*
			*/
			int GetSize() const;

			/**
			* @breif Push value to front of list.
			*
			*/
			void PushFront(const Value & value);

			/**
			* @breif Push value to back of list.
			*
			*/
			void PushBack(const Value & value);

			/**
			* @breif Insert value with given key.
			*
			*/
			void Insert(const std::string & key, const Value & value);

			/**
			* @breif Erase object with an key.
			*
			*/
			Iterator Erase(const std::string & key);

			/**
			* @breif Erase via iterator. Used by lists or objects.
			*
			*/
			Iterator Erase(Iterator & iterator);

			/**
			* @breif Get begining of iterator.
			*
			*/
			Iterator Begin() const;

			/**
			* @breif Get ending of iterator.
			*
			*/
			Iterator End() const;

			/**
			* @breif Assignment operators.
			*
			* @param value Copy another value and all child values to value.
			* @param string Assign string to value.
			* @param number Assign number to value.
			*
			* @return Reference to value. Null value if already assigned once.
			*
			*/
			Value & operator = (const Value & value);
			Value & operator = (const std::string & string);
			Value & operator = (const int number);
			Value & operator = (const long long number);
			Value & operator = (const double number);

			/**
			* @breif Access operators.
			*
			* @param string Access object by key name.
			* @param index Access list item by index.
			*
			* @return Reference to value on success, else Null value.
			*
			*/
			Value & operator [] (const std::string & key);

		private:

			/**
			* @breif Constructor.
			*		 Sets value type.
			*
			* @param type Type of value.
			*
			*/
			Value(const eType type);

			/**
			* @breif Copy value data from another value.
			*
			*/
			void CopyValue(const Value & value);

			/**
			* @breif Delete all allocated memory for this value.
			*
			*/
			void ClearData();

			/**
			* @breif Union containing data.
			*		 Size of 8 byte.
			*
			*/
			union Data
			{
				long long							IntegerData;
				double								FloatData;
				std::string *						StringData;
				std::list<Value *> *				ListData;
				std::map<std::string, Value *> *	ObjectData;
			};

			eType	m_Type; ///< Type of value.
			Data	m_Data; ///< Data union.

		};


		/**
		* @breif YAML reader class.
		*		 Use Reader or Writer function de/serialization.
		*
		*/
		class Reader
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

	}

}