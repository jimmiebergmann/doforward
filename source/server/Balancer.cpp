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

#include <Balancer.hpp>
#include <Exception.hpp>
#include <Yaml.hpp>
#include <algorithm>

namespace dof
{

	// Static declarations
	static bool GetProtocolFromString(std::string & string, Network::Protocol::eType & protocol);
	static bool StringToSeconds(const std::string & string, unsigned int & seconds);

	static Balancer g_DummyBalancer;
	const Service	Balancer::InvalidService(g_DummyBalancer);
	const Node		Balancer::InvalidNode(g_DummyBalancer);

	// Config class
	Balancer::Config::Config()
	{

	}

	Balancer::Config::Config(const Config & config)
	{

	}

	void Balancer::Config::LoadFromFile(const std::string & filename)
	{
		Yaml::Reader reader;
		reader.ReadFromFile(filename, m_ConfigRoot);
	}

	bool Balancer::Config::SaveToFile(const std::string & filename)
	{
		return false;
	}



	// Balancer class
	Balancer::Balancer() :
		m_MaxConnections(0),
		m_InterprocessPort(0)
	{
	}

	Balancer::~Balancer()
	{

	}

	unsigned short Balancer::GetInterprocessPort() const
	{
		return m_InterprocessPort;
	}

	unsigned short Balancer::GetMaxConnections() const
	{
		return m_MaxConnections;
	}

	void Balancer::Run(Config & config)
	{
		LoadConfig(config);

		// ...
	}

	void Balancer::Stop()
	{

	}

	void Balancer::Finish()
	{

	}

	bool Balancer::AddService(Service & service)
	{
		SafeGuard sf1(m_Services);
		SafeGuard sf2(m_ServicesName);
		SafeGuard sf3(m_ServicesPortProto);

		auto it1 = m_Services.Value.find(&service);
		if (it1 != m_Services.Value.end())
		{
			return false;
		}

		auto it2 = m_ServicesName.Value.find(service.GetName());
		if (it2 != m_ServicesName.Value.end())
		{
			return false;
		}

		auto it3 = m_ServicesPortProto.Value.find({ service.GetPeerPort(), service.GetProtocol() });
		if (it3 != m_ServicesPortProto.Value.end())
		{
			return false;
		}

		m_Services.Value.insert(&service);
		m_ServicesName.Value.insert({ service.GetName(), &service });
		m_ServicesPortProto.Value.insert({ { service.GetPeerPort(), service.GetProtocol() }, &service });

		return true;
	}

	bool Balancer::RemoveService(Service & service)
	{
		SafeGuard sf1(m_Services);
		SafeGuard sf2(m_ServicesName);
		SafeGuard sf3(m_ServicesPortProto);

		auto it = m_Services.Value.find(&service);
		if (it == m_Services.Value.end())
		{
			return false;
		}

		Service * pService = &service;

		m_Services.Value.erase(pService);
		m_ServicesName.Value.erase(service.GetName());
		m_ServicesPortProto.Value.erase({ service.GetPeerPort(), service.GetProtocol() });

		delete pService;

		return true;
	}

	Service & Balancer::GetService(const std::string & name)
	{
		SafeGuard sf(m_ServicesName);
		auto it = m_ServicesName.Value.find(name);
		if (it == m_ServicesName.Value.end())
		{
			return const_cast<Service&>(InvalidService);
		}

		return *(it->second);
	}

	Service & Balancer::GetService(const unsigned short port, const Network::Protocol::eType protocol)
	{
		SafeGuard sf(m_ServicesPortProto);
		auto it = m_ServicesPortProto.Value.find({ port, protocol});
		if (it == m_ServicesPortProto.Value.end())
		{
			return const_cast<Service&>(InvalidService);
		}

		return *(it->second);
	}

	bool Balancer::AddNode(Node & node, Service & service)
	{
		SafeGuard sf1(m_Nodes);
		SafeGuard sf2(m_NodesName);
		SafeGuard sf3(m_NodesAddressPortProto);

		auto it1 = m_Nodes.Value.find(&node);
		if (it1 != m_Nodes.Value.end())
		{
			return false;
		}

		auto it2 = m_NodesName.Value.find(node.GetName());
		if (it2 != m_NodesName.Value.end())
		{
			return false;
		}

		auto it3 = m_NodesAddressPortProto.Value.find({ node.GetAddress(), node.GetPort(), node.GetProtocol() });
		if (it3 != m_NodesAddressPortProto.Value.end())
		{
			return false;
		}

		m_Nodes.Value.insert(&node);
		m_NodesName.Value.insert({ node.GetName(), &node });
		m_NodesAddressPortProto.Value.insert({ { node.GetAddress(), node.GetPort(), node.GetProtocol() }, &node });

		service.Associate(node);

		return true;
	}

	bool Balancer::RemoveNode(Node & node)
	{
		SafeGuard sf1(m_Nodes);
		SafeGuard sf2(m_NodesName);
		SafeGuard sf3(m_NodesAddressPortProto);

		auto it = m_Nodes.Value.find(&node);
		if (it == m_Nodes.Value.end())
		{
			return false;
		}

		Node * pNode = &node;
		Service & service = node.GetService();
		if (service != InvalidService)
		{
			service.Detatch(node);
		}

		m_Nodes.Value.erase(pNode);
		m_NodesName.Value.erase(node.GetName());
		m_NodesAddressPortProto.Value.erase({ node.GetAddress(), node.GetPort(), node.GetProtocol() });

		delete pNode;

		return true;
	}

	Node & Balancer::GetNode(const std::string & name)
	{
		SafeGuard sf(m_NodesName);
		auto it = m_NodesName.Value.find(name);
		if (it == m_NodesName.Value.end())
		{
			return const_cast<Node&>(InvalidNode);
		}

		return *(it->second);
	}

	Node & Balancer::GetNode(const std::string & address, const unsigned short port, const Network::Protocol::eType protocol)
	{
		SafeGuard sf(m_NodesAddressPortProto);
		auto it = m_NodesAddressPortProto.Value.find({ address, port, protocol });
		if (it == m_NodesAddressPortProto.Value.end())
		{
			return const_cast<Node&>(InvalidNode);
		}

		return *(it->second);
	}

	Balancer::Balancer(const Balancer & balancer)
	{

	}

	void Balancer::LoadConfig(Config & config)
	{
		Yaml::Node & conf_root = config.m_ConfigRoot;
		Yaml::Node & conf_server = conf_root["server"];
		Yaml::Node & conf_services = conf_root["services"];

		if (conf_server.IsMapping())
		{
			m_MaxConnections = conf_server["max_connections"].Value<unsigned short>(1024);
			m_InterprocessPort = conf_server["com_port"].Value<unsigned short>(240);
		}
		if (conf_services.IsSequence())
		{
			for (auto i = 0; i < conf_services.Size(); i++)
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

	void Balancer::LoadConfigService(Yaml::Node & service, const unsigned int index)
	{
		// Get and validate service data.
		const std::string default_name = GetNextServiceName();
		std::string name				= service["name"].Value<std::string>(default_name);
		std::string protocol_str		= service["protocol"].Value<std::string>("");
		unsigned short peer_port		= service["peer_port"].Value<unsigned short>(0);
		std::string monitor				= service["monitor"].Value<std::string>("");
		unsigned short monitor_port		= service["monitor_port"].Value<unsigned short>(0);
		unsigned int max_connections	= service["max_connections"].Value<unsigned short>(256);
		std::string session				= service["session"].Value<std::string>("15m");

		if (name.size() == 0)
		{
			name = default_name;
		}
		Network::Protocol::eType protocol;
		if (protocol_str.size() == 0)
		{
			throw Yaml::ParsingError("Config - Protocol of service no." + std::to_string(index) + " is missing.");
		}
		else
		{
			if (GetProtocolFromString(protocol_str, protocol) == false)
			{
				throw Exception(Exception::ValidationError, "Config - Invalid protocol of service no. " + std::to_string(index) + ": " + protocol_str);
			}
		}
		if (peer_port == 0)
		{
			throw Exception(Exception::ValidationError, "Config - Peer port of service no. " + std::to_string(index) + " is missing or 0.");
		}
		std::transform(monitor.begin(), monitor.end(), monitor.begin(), ::tolower);
		if (monitor == "disabled" || monitor == "false" || monitor == "0")
		{
			monitor_port = 0;
		}

		std::transform(session.begin(), session.end(), session.begin(), ::tolower);
		unsigned session_seconds = 0;
		if (session == "disabled" || session == "false" || session == "0")
		{
			session_seconds = 0;
		}
		else if (StringToSeconds(session, session_seconds) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Session of service no. " + std::to_string(index) + " is invalid: " + session);
		}

		// Create and add service.
		Service * pService = new Service(*this, name, protocol, peer_port, monitor_port, session_seconds, max_connections);
		AddService(*pService);

		// Go through service nodes
		Yaml::Node & conf_nodes = service["nodes"];
		if (conf_nodes.IsSequence())
		{
			for (auto i = 0; i < conf_nodes.Size(); i++)
			{
				Yaml::Node & currentNode = conf_nodes[i];
				LoadConfigNode(currentNode, *pService, i, index);
			}
		}
		else if (conf_nodes.IsMapping())
		{
			LoadConfigNode(conf_nodes, *pService, 0, index);
		}
	}

	void Balancer::LoadConfigNode(Yaml::Node & node, Service & service, const unsigned int index, const unsigned int service_index)
	{
		// Get and validate node data.
		const std::string default_name = GetNextNodeName();
		std::string name =			node["name"].Value<std::string>(default_name);
		std::string protocol_str =  node["protocol"].Value<std::string>("");
		std::string ip =			node["ip"].Value<std::string>("");
		unsigned short port =		node["port"].Value<unsigned short>(0);

		if (name.size() == 0)
		{
			name = default_name;
		}
		Network::Protocol::eType protocol;
		if (protocol_str.size() == 0)
		{
			throw Yaml::ParsingError("Config - Protocol of service no. " + std::to_string(index) + " is missing.");
		}
		else
		{
			if (GetProtocolFromString(protocol_str, protocol) == false)
			{
				throw Exception(Exception::ValidationError, "Config - Invalid protocol of service no." + std::to_string(index) + ": " + protocol_str);
			}
		}
		if (ip.size() == 0)
		{
			throw Exception(Exception::ValidationError, "Config - Ip of node no. " + std::to_string(index) +
										" (service no. " + std::to_string(service_index) + ") is missing.");
		}
		if (port == 0)
		{
			throw Exception(Exception::ValidationError, "Config - Port of node no. " + std::to_string(index) +
										" (service no. " + std::to_string(service_index) + ") is missing or 0.");
		}

		// Create node
		Node * pNode = new Node(*this, name, protocol, ip, port);
		if (AddNode(*pNode, service) == false)
		{
			throw Exception(Exception::ValidationError, "Config - Duplicate of node, for service no. " + std::to_string(index) + ".");
		}

	}

	std::string Balancer::GetNextServiceName()
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

	std::string Balancer::GetNextNodeName()
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


	// Static implementaitons
	bool GetProtocolFromString(std::string & string, Network::Protocol::eType & protocol)
	{
		std::transform(string.begin(), string.end(), string.begin(), ::tolower);

		if (string == "tcp")
		{
			protocol = Network::Protocol::Tcp;
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

}
