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

#include <network/Socket.hpp>

namespace dof
{

	namespace Network
	{

		/**
		* @breif Tcp socket class. Used for hosting, connecting, sending and receiving data.
		*		 When connecting, the type of socket is set to "Peer", which means
		*		 the "Host" functions will generate Exceptions if called.
		*
		* @see Socket
		*
		*/
		class TcpSocket : public Socket
		{

		public:

			/**
			* @breif Enumueration of types of sockets.
			*
			*/
			enum eType
			{
				None,
				Host,
				Peer
			};

			/**
			* @breif Constructor.
			*
			* @param port	Binding port. 0 means "any".
			* @param handle	Socket handle.
			* @param type	Type of socket.
			*
			*/
			TcpSocket(const unsigned short port = 0);
			TcpSocket(const Handle handle, const eType type);

			/**
			* @breif Destructor.
			*
			*/
			~TcpSocket();

			/**
			* @breif Get bound port.
			*
			*/
			virtual unsigned short GetPort();

			/**
			* @breif Close socket.
			*		 Same as Disconnect / Mute.
			*		 Will turn the socket into None type socket.
			*
			*/
			void Close();

			/**
			* @breif Connect to host.
			*		 Will turn the socket into Peer type socket.
			*
			* @param host	Address to host.
			* @param port	Port of host.
			* @param bind	Specifies binding port. 0 for any, using constructor port if 0.
			*				"Any" port if both are 0.
			*
			*/
			bool Connect(const Address & host, const unsigned int port, const unsigned int bind = 0);

			/**
			* @breif Listen for connections.
			*		 Will turn the socket into Host type socket.
			*
			* @param port	Specifies binding port. 0 for any, using constructor port if 0.
			*				"Any" port if both are 0.
			*
			*/
			void Listen(const unsigned int port = 0, const Network::Address & address = Network::Address::EmptyIpv4);

			/**
			* @breif Receive data.
			*
			* @param buffer Pointer to data buffer.
			* @param size	Max size of buffer.
			*
			* @return Size of received data, 0 if disconnected, -1 if failed.
			*
			*/
			int Receive(char * buffer, int size);

			/**
			* @breif Send data.
			*
			* @param buffer Pointer to data buffer.
			* @param size	Size of data to send..
			*
			* @return Size of sent data, 0 if disconnected, -1 if failed.
			*
			*/
			int Send(const char * buffer, int size);

		private:

			eType			m_Type;	///< Type of socket.
			unsigned short	m_Port;	///< Socket port.

		};

	}

}
