/*

MIT License

Copyright (c) 2018 Jimmie Bergmann

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

#include <network/Address.hpp>
#include <Exception.hpp>
#include <sstream>
/*#include <memory>
#include <WinSock2.h>
#include <Ws2tcpip.h>*/

namespace dof
{

	namespace Network
	{

		const Address Address::EmptyIpv4 = Address(static_cast<unsigned int>(0));

		Address::Address(const eType p_Type) :
			m_Type(p_Type)
		{
			memset(m_Bytes, 0, 16);
		}

		Address::Address(const Address & p_Address) :
			m_Type(p_Address.m_Type)
		{
			memcpy(m_Bytes, p_Address.m_Bytes, 16);
			m_Type = p_Address.m_Type;
		}

		Address::Address(const unsigned int p_Address) :
			m_Type(Ipv4)
		{
			memcpy(m_Bytes, &p_Address, 4);
		}

		/*Address::Address(const unsigned char * p_pSourceBytes, const unsigned int p_ByteCount) :
			m_Type(Ipv6)
		{
			if (p_ByteCount > 16)
			{
				throw Exception(Exception::InvalidInput, "Overflow: p_ByteCount is larger than 16.");
			}

			memcpy(m_Bytes, p_pSourceBytes, p_ByteCount);
		}

		Address::Address(const std::initializer_list<unsigned char> & p_Bytes)
		{
			if (p_Bytes.size() <= 4)
			{
				m_Type == Ipv4;
			}
			else
			{
				m_Type == Ipv6;
			}

			size_t counter = 0;
			for (auto it = p_Bytes.begin(); it != p_Bytes.end() && counter < 16; it++, counter++)
			{
				m_Bytes[counter] = *it;
			}

		}*/

		const unsigned char Address::GetType() const
		{
			return m_Type;
		}

		Address & Address::GetByName(const std::string & name)
		{
			std::stringstream ss(name);
			char dot[3];		
			unsigned int part[4];

			ss >> part[0] >> dot[0] >> part[1] >> dot[1] >> part[2] >> dot[2] >> part[3];
			if (ss.fail() || ss.eof() == false)
			{
				return *this = Address(Invalid);
			}

			const bool isIpv4 = dot[0] == '.' && dot[1] == '.' && dot[2] == '.';
			const bool validParts = part[0] < 256 && part[1] < 256 && part[2] < 256 && part[3] < 256;
			if(isIpv4 == false || validParts == false)
			{
				return *this = Address(Invalid);
			}

			// Valid Ipv4 address.
			m_Type = Ipv4;
			for (size_t i = 0; i < 4; i++)
			{
				m_Bytes[i] = static_cast<unsigned char>(part[i]);
			}

			return *this;
		}

		std::string Address::AsString() const
		{
			if (m_Type == Invalid)
			{
				return "";
			}

			if (m_Type == Ipv6)
			{
				Exception(Exception::InvalidType, "Ipv6 is not yet implemented.");
				return "";
			}


			// Ipv4
			std::stringstream ss;
			ss << (int)m_Bytes[0] << "." << (int)m_Bytes[1] << "." << (int)m_Bytes[2] << "." << (int)m_Bytes[3];
			return std::string(ss.str());
		}

		unsigned char Address::GetByte(const unsigned int p_ByteIndex) const
		{
			if (p_ByteIndex > 16)
			{
				Exception(Exception::InvalidInput, "Overflow: p_ByteIndex is larger than 16.");
			}

			return m_Bytes[p_ByteIndex];
		}

		void Address::GetBytes(unsigned char * p_DestinationBytes, const unsigned int p_ByteCount) const
		{
			if (p_ByteCount > 16)
			{
				Exception(Exception::InvalidInput, "Overflow: p_ByteCount is larger than 16.");
			}

			memcpy(p_DestinationBytes, m_Bytes, p_ByteCount);
		}

		void Address::SetByte(const unsigned char p_Byte, const unsigned int p_ByteIndex)
		{
			if (p_ByteIndex >= 16)
			{
				Exception(Exception::InvalidInput, "Overflow: p_ByteIndex is larger than 16.");
			}

			m_Bytes[p_ByteIndex] = p_Byte;
		}

		void Address::SetBytes(const unsigned char * p_SourceBytes, const unsigned int p_ByteCount)
		{
			if (p_ByteCount >= 16)
			{
				Exception(Exception::InvalidInput, "Overflow: p_ByteCount is larger than 16.");
			}

			memcpy(m_Bytes, p_SourceBytes, p_ByteCount);
		}

		bool Address::operator == (const Address & address)
		{
			if (m_Type != address.m_Type)
			{
				return false;
			}

			const size_t dataSize = m_Type == Ipv4 ? 4 : 16;
			return memcmp(m_Bytes, address.m_Bytes, dataSize) == 0;
		}

		bool Address::operator != (const Address & address)
		{
			return !(*this == address);
		}

		bool operator < (const Address & left, const Address & right)
		{
			for (size_t i = 0; i < 16; i++)
			{
				if (left.m_Bytes[i] < right.m_Bytes[i])
				{
					return true;
				}
			}
			
			return false;
		}

		bool operator > (const Address & left, const Address & right)
		{
			return left < right;
		}

	}

}

namespace std
{

	stringstream & operator >> (std::stringstream &in, dof::Network::Address & address)
	{
		const std::string str = in.str();
		address.GetByName(str);
		return in;
	}

}



