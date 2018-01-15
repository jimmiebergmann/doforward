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

#include <Server.hpp>
#include <services/TcpService.hpp>
#include <services/UdpService.hpp>
#include <Balancer.hpp>
#include <Exception.hpp>
#include <Yaml.hpp>
#include <algorithm>

namespace dof
{

	// Static declarations
	static bool GetProtocolsFromString( std::string & string,
										Network::Protocol::eTransport & transportProtocol,
										Network::Protocol::eApplication & applicationProtocol);
	static bool GetBalancerAlgorithmFromString(	const std::string & string,
												Balancer::eAlgorithm & algorithm);
	static bool StringToSeconds(const std::string & string, unsigned int & seconds);


	// Config class
	Server::Config::Config()
	{

	}

	Server::Config::Config(const Config & config)
	{

	}

	void Server::Config::LoadFromFile(const std::string & filename)
	{
		Yaml::Reader reader;
		reader.ReadFromFile(filename, m_ConfigRoot);
	}

	bool Server::Config::SaveToFile(const std::string & filename)
	{
		return false;
	}



	// Balancer class
	Server::Server() :
		m_MaxConnections(0),
		m_InterprocessPort(0)
	{
	}

	Server::~Server()
	{

	}

	unsigned short Server::GetInterprocessPort() const
	{
		return m_InterprocessPort;
	}

	unsigned short Server::GetMaxConnections() const
	{
		return m_MaxConnections;
	}

	void Server::Run(Config & config)
	{
		Network::Initialize();

		LoadConfig(config);
		
		for (auto it = m_Services.Value.begin(); it != m_Services.Value.end(); it++)
		{
			(*it)->Start();
		}
	}

	void Server::Stop()
	{
		m_StopSemaphore.NotifyOne();
	}

	void Server::Finish()
	{
		m_StopSemaphore.Wait();
	}

	bool Server::AddService(Service * service)
	{
		if (service == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Service pointer is nullptr.");
		}

		SafeGuard sf1(m_Services);
		SafeGuard sf2(m_ServicesName);
		SafeGuard sf3(m_ServicesHost);

		auto it1 = m_Services.Value.find(service);
		if (it1 != m_Services.Value.end())
		{
			return false;
		}

		auto it2 = m_ServicesName.Value.find(service->GetName());
		if (it2 != m_ServicesName.Value.end())
		{
			return false;
		}

		auto it3 = m_ServicesHost.Value.find({ service->GetHost(), service->GetPort(), service->GetTransportProtocol(), service->GetApplicationProtocol() });
		if (it3 != m_ServicesHost.Value.end())
		{
			return false;
		}

		m_Services.Value.insert(service);
		m_ServicesName.Value.insert({ service->GetName(), service });
		m_ServicesHost.Value.insert({ { service->GetHost(), service->GetPort(), service->GetTransportProtocol(), service->GetApplicationProtocol() }, service });

		return true;
	}

	bool Server::RemoveService(Service * service)
	{
		if (service == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Service pointer is nullptr.");
		}

		SafeGuard sf1(m_Services);
		SafeGuard sf2(m_ServicesName);
		SafeGuard sf3(m_ServicesHost);

		auto it = m_Services.Value.find(service);
		if (it == m_Services.Value.end())
		{
			return false;
		}

		std::set<Node *> nodeSet;
		service->GetNodes(nodeSet);
		for (auto it = nodeSet.begin(); it != nodeSet.end(); it++)
		{
			Node * pNode = *it;
			if (RemoveNode(pNode) == false)
			{
				throw Exception(Exception::InvalidInput, "Node associated with unknown node.");
			}
		}

		m_Services.Value.erase(service);
		m_ServicesName.Value.erase(service->GetName());
		m_ServicesHost.Value.erase({ service->GetHost(), service->GetPort(), service->GetTransportProtocol(), service->GetApplicationProtocol() });

		delete service;

		return true;
	}

	Service * Server::GetService(const std::string & name)
	{
		SafeGuard sf(m_ServicesName);
		auto it = m_ServicesName.Value.find(name);
		if (it == m_ServicesName.Value.end())
		{
			return nullptr;
		}

		return it->second;
	}

	Service * Server::GetService(const Network::Address & host,
							     const unsigned short port,
								 const Network::Protocol::eTransport transportProtocol,
								 const Network::Protocol::eApplication applicationProtocol)
	{
		SafeGuard sf(m_ServicesHost);
		auto it = m_ServicesHost.Value.find({ host, port, transportProtocol, applicationProtocol });
		if (it == m_ServicesHost.Value.end())
		{
			return nullptr;
		}

		return it->second;
	}

	bool Server::AddNode(Node * node, Service * service)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}
		if (service == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Service pointer is nullptr.");
		}

		SafeGuard sf1(m_Nodes);
		SafeGuard sf2(m_NodesName);
		SafeGuard sf3(m_NodesHost);

		auto it1 = m_Nodes.Value.find(node);
		if (it1 != m_Nodes.Value.end())
		{
			return false;
		}

		auto it2 = m_NodesName.Value.find(node->GetName());
		if (it2 != m_NodesName.Value.end())
		{
			return false;
		}

		auto it3 = m_NodesHost.Value.find({ node->GetHost(), node->GetPort(), node->GetTransportProtocol(), node->GetApplicationProtocol() });
		if (it3 != m_NodesHost.Value.end())
		{
			return false;
		}

		m_Nodes.Value.insert(node);
		m_NodesName.Value.insert({ node->GetName(), node });
		m_NodesHost.Value.insert({ { node->GetHost(), node->GetPort(), node->GetTransportProtocol(), node->GetApplicationProtocol() }, node });

		service->Associate(node);

		return true;
	}

	bool Server::RemoveNode(Node * node)
	{
		if (node == nullptr)
		{
			throw Exception(Exception::InvalidInput, "Node pointer is nullptr.");
		}

		SafeGuard sf1(m_Nodes);
		SafeGuard sf2(m_NodesName);
		SafeGuard sf3(m_NodesHost);

		auto it = m_Nodes.Value.find(node);
		if (it == m_Nodes.Value.end())
		{
			return false;
		}

		Service * pService = node->GetService();
		if (pService)
		{
			pService->Detatch(node);
		}
		
		m_Nodes.Value.erase(node);
		m_NodesName.Value.erase(node->GetName());
		m_NodesHost.Value.erase({ node->GetHost(), node->GetPort(), node->GetTransportProtocol(), node->GetApplicationProtocol() });

		delete node;

		return true;
	}

	Node * Server::GetNode(const std::string & name)
	{
		SafeGuard sf(m_NodesName);
		auto it = m_NodesName.Value.find(name);
		if (it == m_NodesName.Value.end())
		{
			return nullptr;
		}

		return it->second;
	}

	Node * Server::GetNode(const Network::Address & address,
						   const unsigned short port,
						   const Network::Protocol::eTransport transportProtocol,
						   const Network::Protocol::eApplication applicationProtocol)
	{
		SafeGuard sf(m_NodesHost);
		auto it = m_NodesHost.Value.find({ address, port, transportProtocol, applicationProtocol });
		if (it == m_NodesHost.Value.end())
		{
			return nullptr;
		}

		return it->second;
	}

	Server::Server(const Server & server)
	{

	}

	void Server::LoadConfig(Config & config)
	{
		Yaml::Node & conf_root = config.m_ConfigRoot;
		Yaml::Node & conf_server = conf_root["server"];
		Yaml::Node & conf_services = conf_root["services"];

		if (conf_server.IsMapping())
		{
			m_MaxConnections = conf_server["max_connections"].Value<unsigned short>(1024);
			m_InterprocessPort = conf_server["com_port"].Value<unsigned short>(240);
			LoadDefaultServiceConifg(conf_server);
		}
		if (conf_services.IsSequence())
		{
			for (size_t i = 0; i < conf_services.Size(); i++)
			{
				Yaml::Node & currentService = conf_services[i];
				LoadConfigService(currentService, i);
			}
		}
		else if (conf_services.IsMapping())
		{
			LoadConfigService(conf_services, 0);
		}
	}

	void Server::LoadConfigService(Yaml::Node & service, const unsigned int index)
	{
		// Get and validate service data.
		const std::string default_name	= GetNextServiceName();
		std::string name				= service["name"].Value<std::string>();
		std::string protocol_str		= service["protocol"].Value<std::string>("");
		Network::Address host			= service["host"].Value<Network::Address>(Network::Address::EmptyIpv4);
		unsigned short port				= service["port"].Value<unsigned short>(0);
		std::string balancing			= service["balancing"].Value<std::string>("");
		/*Yaml::Node & monitor			= service["monitor"];
		unsigned int max_connections	= service["max_connections"].Value<unsigned short>(256);
		std::string session				= service["session"].Value<std::string>("");*/

		Service::Config serviceConfig = m_DefaultServiceConfig;

		if (name.size() == 0)
		{
			name = default_name;
		}
		Network::Protocol::eTransport transportProtocol;
		Network::Protocol::eApplication applicationProtocol;
		if (protocol_str.size() == 0)
		{
			throw Yaml::ParsingError("Config - Protocol of service no." + std::to_string(index) + " is missing.");
		}
		else
		{
			if (GetProtocolsFromString(protocol_str, transportProtocol, applicationProtocol) == false)
			{
				throw Exception(Exception::ValidationError, "Config - Invalid protocol of service no. " + std::to_string(index) + ": " + protocol_str);
			}
		}
		if (host.GetType() == Network::Address::Invalid)
		{
			throw Exception(Exception::ValidationError, "Config - Invalid host address of service no. " + std::to_string(index) + ".");
		}
		if (port == 0)
		{
			throw Exception(Exception::ValidationError, "Config - Port of service no. " + std::to_string(index) + " is missing or 0.");
		}
		Balancer::eAlgorithm balancingAlgorithm = Balancer::RoundRobin;
		if (GetBalancerAlgorithmFromString(balancing, balancingAlgorithm) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Balancing algorithm of service no. " + std::to_string(index) + " is invalid: " + balancing);
		}

		serviceConfig.Name = name;
		serviceConfig.Host = host;
		serviceConfig.Port = port;
		serviceConfig.BalancerAlgorithm = balancingAlgorithm;

		/*
		///< MONITOR NOT YET IMPLEMENTED!
			bool invalidMonitor = false;
			if (monitor.IsMapping())
			{

			}
			else
			{
			
			}
			if (invalidMonitor)
			{
				throw Exception(Exception::ValidationError, "Config - Invalid montior of service no. " + std::to_string(index) + ".");
			}
		///< MONITOR NOT YET IMPLEMENTED!


		std::transform(session.begin(), session.end(), session.begin(), ::tolower);
		unsigned int session_seconds = 0;
		if (session == "" || session == "disabled" || session == "false")
		{
			session_seconds = 0;
		}
		else if (StringToSeconds(session, session_seconds) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Session of service no. " + std::to_string(index) + " is invalid: " + session);
		}

		*/

		// Create and add service.
		Service * pService = CreateService(transportProtocol, applicationProtocol, serviceConfig);
		if (AddService(pService) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Duplicate of service no. " + std::to_string(index) + ".");
		}

		// Go through service nodes
		Yaml::Node & conf_nodes = service["nodes"];
		if (conf_nodes.IsSequence())
		{
			for (size_t i = 0; i < conf_nodes.Size(); i++)
			{
				Yaml::Node & currentNode = conf_nodes[i];
				LoadConfigNode(currentNode, pService, i, index);
			}
		}
		else if (conf_nodes.IsMapping())
		{
			LoadConfigNode(conf_nodes, pService, 0, index);
		}
	}

	void Server::LoadConfigNode(Yaml::Node & node, Service * service, const unsigned int index, const unsigned int service_index)
	{
		// Get and validate node data.
		const std::string default_name = GetNextNodeName();
		std::string name =			node["name"].Value<std::string>();
		std::string protocol_str =  node["protocol"].Value<std::string>("");
		Network::Address host =		node["host"].Value<Network::Address>(Network::Address::EmptyIpv4);
		unsigned short port =		node["port"].Value<unsigned short>(0);

		if (name.size() == 0)
		{
			name = default_name;
		}
		Network::Protocol::eTransport transportProtocol;
		Network::Protocol::eApplication applicationProtocol;
		if (protocol_str.size() == 0)
		{
			throw Yaml::ParsingError("Config - Protocol of service no. " + std::to_string(index) + " is missing.");
		}
		else
		{
			if (GetProtocolsFromString(protocol_str, transportProtocol, applicationProtocol) == false)
			{
				throw Exception(Exception::ValidationError, "Config - Invalid protocol of service no." + std::to_string(index) + ": " + protocol_str);
			}
		}
		if (host.GetType() == Network::Address::Invalid)
		{
			throw Exception(Exception::ValidationError, "Config - Invalid host address of node no. " + std::to_string(index) +
														" (service no. " + std::to_string(service_index) + ") is missing or 0.");
		}
		if (port == 0)
		{
			throw Exception(Exception::ValidationError, "Config - Port of node no. " + std::to_string(index) +
														" (service no. " + std::to_string(service_index) + ") is missing or 0.");
		}

		// Create node
		Node * pNode = new Node(*this, name, transportProtocol, applicationProtocol, host, port);
		if (AddNode(pNode, service) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Duplicate of node, for service no. " + std::to_string(index) + ".");
		}

	}

	void Server::LoadDefaultServiceConifg(Yaml::Node & server)
	{
		m_DefaultServiceConfig.BalancerAlgorithm = Balancer::RoundRobin;
		m_DefaultServiceConfig.SessionTimeout = Time::Zero;
		m_DefaultServiceConfig.MaxConnections = 1024;

		m_DefaultServiceConfig.BufferInfo.Size = 8192;
		m_DefaultServiceConfig.BufferInfo.PoolCount = 512;
		m_DefaultServiceConfig.BufferInfo.PoolReserveCount = 128;
		m_DefaultServiceConfig.BufferInfo.PoolMaxCount = 2048;
		m_DefaultServiceConfig.BufferInfo.PoolAllocationCount = 32;
	}

	std::string Server::GetNextServiceName()
	{
		SafeGuard sf(m_ServicesName);

		std::string name = "Service " + std::to_string(m_ServicesName.Value.size());
		auto it = m_ServicesName.Value.find(name);
		if (it == m_ServicesName.Value.end())
		{
			return name;
		}

		// We got an duplicate of the name, add post-fix.
		name += "#";
		size_t loops = 2;
		while (1)
		{
			std::string newName = name + std::to_string(loops);
			auto it = m_ServicesName.Value.find(newName);
			if (it == m_ServicesName.Value.end())
			{
				return newName;
			}
			loops++;
		}

		return "";
	}

	std::string Server::GetNextNodeName()
	{
		SafeGuard sf(m_NodesName);

		std::string name = "Node " + std::to_string(m_NodesName.Value.size());
		auto it = m_NodesName.Value.find(name);
		if (it == m_NodesName.Value.end())
		{
			return name;
		}

		// We got an duplicate of the name, add post-fix.
		name += "#";
		size_t loops = 2;
		while (1)
		{
			std::string newName = name + std::to_string(loops);
			auto it = m_NodesName.Value.find(newName);
			if (it == m_NodesName.Value.end())
			{
				return newName;
			}
			loops++;
		}

		return "";
	}


	Service * Server::CreateService(const Network::Protocol::eTransport transportProtocol,
									const Network::Protocol::eApplication applicationProtocol,
									const Service::Config & config)
	{
		Service * pService = nullptr;

		switch (transportProtocol)
		{
			case Network::Protocol::Tcp:
			{
				switch (applicationProtocol)
				{
				case Network::Protocol::None:
					pService = new TcpService(*this, config);
					break;
				default:
					throw Exception(Exception::InvalidInput, "Not yet supported application protocol.");
					break;
				}
			}
			break;
			case Network::Protocol::Udp:
			{
				if (applicationProtocol != Network::Protocol::None)
				{
					throw Exception(Exception::InvalidInput, "No application protocols are supported for UDP.");
				}
				throw Exception(Exception::InvalidInput, "Udp services are not yet implemented.");
			}
			break;
			default:
				throw Exception(Exception::InvalidInput, "Unknown protocol.");
				break;
		}

		return pService;
	}


	// Static implementaitons
	bool GetProtocolsFromString(std::string & string,
		Network::Protocol::eTransport & transportProtocol,
		Network::Protocol::eApplication & applicationProtocol)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);

		if (string == "tcp")
		{
			transportProtocol = Network::Protocol::Tcp;
			applicationProtocol = Network::Protocol::None;
			return true;
		}
		else if (string == "udp")
		{
			transportProtocol = Network::Protocol::Udp;
			applicationProtocol = Network::Protocol::None;
			return true;
		}
		else if (string == "http")
		{
			transportProtocol = Network::Protocol::Tcp;
			applicationProtocol = Network::Protocol::Http;
			return true;
		}
		else if (string == "https")
		{
			transportProtocol = Network::Protocol::Tcp;
			applicationProtocol = Network::Protocol::Https;
			return true;
		}

		return false;
	}

	bool StringToSeconds(const std::string & string, unsigned int & seconds)
	{
		if (string.size() == 0)
		{
			return false;
		}

		size_t firstChar = string.find_first_not_of("1234567890 ");
		if (firstChar == 0)
		{
			return false;
		}

		std::stringstream ss(string);
		ss >> seconds;
		
		if (firstChar == std::string::npos)
		{
			return true;
		}

		std::string unit = string.substr(firstChar);
		std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);
		if (unit == "s")
		{
			return true;
		}
		else if (unit == "m")
		{
			seconds *= 60;
		}
		else if (unit == "h")
		{
			seconds *= 60 * 60;
		}
		else if (unit == "d")
		{
			seconds *= 60 * 60 * 24;
		}
		else
		{
			return false;
		}

		return true;
	}

	bool GetBalancerAlgorithmFromString(const std::string & string, Balancer::eAlgorithm & algorithm)
	{
		std::string algStr = string;
		algStr.erase(remove_if(algStr.begin(), algStr.end(), ::isspace), algStr.end());
		std::transform(algStr.begin(), algStr.end(), algStr.begin(), ::tolower);

		if (algStr == "" || algStr == "roundrobin" || algStr == "rr")
		{
			algorithm = Balancer::RoundRobin;
		}
		else if (algStr == "connectioncount" || algStr == "cc")
		{
			algorithm = Balancer::ConnectionCount;
		}
		else
		{
			return false;
		}

		return true;
	}

}
