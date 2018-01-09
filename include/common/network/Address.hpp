/*

MIT License

Copyright (c) 2017 Jimmie Bergmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <set>
#include <string>
#include <sstream>
#include <initializer_list>


namespace dof
{

	namespace Network
	{

		/**
		* @breif Address class.
		*
		*/
		class Address
		{

		public:

			/**
			* @breif Type of IP address.
			*
			*/
			enum eType
			{
				Invalid,
				Ipv4,
				Ipv6
			};

			static const Address EmptyIpv4;

			Address(const eType p_Type = Invalid);
			Address(const Address & p_Address);
			Address(const unsigned int p_Address);
			/*Address(const unsigned char * p_pSourceBytes, const unsigned int p_ByteCount);
			Address(const std::initializer_list<unsigned char> & p_Bytes);*/

			const unsigned char GetType() const;

			Address & GetByName(const std::string & p_Name);

			std::string AsString() const;

			unsigned char GetByte(const unsigned int p_ByteIndex) const;
			void GetBytes(unsigned char * p_DestinationBytes, const unsigned int p_ByteCount) const;
			void SetByte(const unsigned char p_Byte, const unsigned int p_ByteIndex);
			void SetBytes(const unsigned char * p_SourceBytes, const unsigned int p_ByteCount);

			bool operator == (const Address & address);
			bool operator != (const Address & address);
			friend bool operator < (const Address & left, const Address & right);
			friend bool operator > (const Address & left, const Address & right);

		private:

			eType			m_Type;			///< Type of address.
			unsigned char	m_Bytes[16];	///< Bytes for version 4 and 6 address.

		};

	}

}

namespace std
{
	stringstream & operator >> (std::stringstream &in, dof::Network::Address & address);
}