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

#include <network/TcpSocket.hpp>
#include <Exception.hpp>

namespace dof
{

	namespace Network
	{

		TcpSocket::TcpSocket(const unsigned short port) :
			Socket(),
			m_Type(None),
			m_Port(port)
		{
		}

		TcpSocket::TcpSocket(const Handle handle, const eType type) :
			Socket(),
			m_Type(type),
			m_Port(0)
		{
			m_Handle = handle;
		}

		TcpSocket::~TcpSocket()
		{
			Close();
		}

		unsigned short TcpSocket::GetPort()
		{
			if (m_Handle == 0)
			{
				return 0;
			}

			return m_Port;
		}

		void TcpSocket::Close()
		{
			if (m_Handle)
			{
				closesocket(m_Handle);
				m_Handle = 0;
			}

			m_Type = None;
		}

		bool TcpSocket::Connect(const Address & host, const unsigned int port, const unsigned int bind)
		{
			if (m_Type != None)
			{
				throw new Exception(Exception::InvalidType, std::string("Connect: Invalid Socket type: ") + std::to_string(m_Type));
			}

			m_Type = Peer;
			return false;
		}


		void TcpSocket::Listen(const unsigned int port, const Network::Address & address)
		{
			if (m_Type != None)
			{
				throw new Exception(Exception::InvalidType, std::string("Listen: Invalid Socket type: ") + std::to_string(m_Type));
			}

			// Get port
			unsigned short bindPort = m_Port;
			if (bindPort == 0)
			{
				if (port == 0)
				{
					throw new Exception(Exception::InvalidInput, "Invalid listen port: " + std::to_string(bindPort));
				}
				bindPort = port;
			}
			
			// Create socket
			if ((m_Handle = socket(AF_INET, SOCK_STREAM, 0)) == 0)
			{
				throw new Exception(Exception::Network, "Failed to create listen socket. Error no. " + std::to_string(GetLastError()));
			}
			const int opt = 1;
			if (setsockopt(m_Handle, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
			{
				throw new Exception(Exception::Network, "Failed to make listen socket reusable. Error no. " + std::to_string(GetLastError()));
			}

			// Bind socket
			struct sockaddr_in bindAddress;
			bindAddress.sin_family = AF_INET;
			bindAddress.sin_addr.s_addr = INADDR_ANY;
			bindAddress.sin_port = htons(bindPort);
			if (bind(m_Handle, (struct sockaddr *)&bindAddress, sizeof(bindAddress)) < 0)
			{
				throw new Exception(Exception::Network, "Failed to bind listen socket. Error no. " + std::to_string(GetLastError()));
			}

			// Listen
			if (listen(m_Handle, SOMAXCONN) < 0)
			{
				throw new Exception(Exception::Network, "Failed to listen on socket. Error no. " + std::to_string(GetLastError()));
			}

			m_Port = bindPort;
			m_Type = Host;
		}

		int TcpSocket::Receive(char * buffer, int size)
		{
			return recv(m_Handle, buffer, size, 0);
		}

		int TcpSocket::Send(const char * buffer, int size)
		{
			return send(m_Handle, buffer, size, 0);
		}
	}

}