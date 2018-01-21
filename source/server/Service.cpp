/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
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

#include <Service.hpp>

namespace dof
{

	// Service config struct
	Service::Config::Config() :
		Name(""),
		Host(0),
		Port(0),
		BufferInfo(),
		BalancerAlgorithm(Balancer::RoundRobin),
		SessionTimeout(Microseconds(0)),
		MaxConnections(1024)
	{

	}

	Service::Config::Buffer::Buffer() :
		Size(8192),
		PoolCount(10),
		PoolMaxCount(10),
		PoolReserveCount(1),
		PoolAllocationCount(3)
	{

	}

	// Service class
	Service::Service(Server & server,
					 const Config & config) :
		m_Server(server),
		m_Config(config)
	{
	};

	Service::~Service()
	{
	}

	Server & Service::GetServer() const
	{
		return m_Server;
	}

	const Service::Config & Service::GetConfig() const
	{
		return m_Config;
	}

	const std::string & Service::GetName() const
	{
		return m_Config.Name;
	}

	const Network::Address & Service::GetHost() const
	{
		return m_Config.Host;
	}

	unsigned short Service::GetPort() const
	{
		return m_Config.Port;
	}

	const Time & Service::GetSessionTimeout() const
	{
		return m_Config.SessionTimeout;
	}

	unsigned short Service::GetMaxConnections() const
	{
		return m_Config.MaxConnections;
	}

}