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

#include <Service.hpp>
#include <Node.hpp>
#include <Safe.hpp>
#include <Yaml.hpp>
#include <string>
#include <map>

namespace dof
{

	/**
	* @breif Main class of DoForward server.
	*		 The balancer will load configurations, balance connections and packets,
	*		 handle disconnections, gather statistics, etc...
	*
	*		 All nodes and groups are acknowledged, created, and destroyed by the balancer class.
	*
	* @see Service
	* @see Node
	* @see Group
	*
	*/
	class Balancer
	{

	public:

		/**
		* @breif Configuration class of the balancer,
		*		 including specifying nodes, protocols, ports, etc...
		*
		*		 Full structure of file, with example data.
		*		 - Default values are within asterisks.
		*		 - Multi-option values are within parentheses.
		*		 - Mandatory fields are within slashes.
		*		 ---------------------------------------------------
		*			/server/:
		*				- max_connections: *1024*
		*
		*			/services/:
		*				- name:				  service1
		*				  protocol:			  tcp (tcp/udp/http/https)
		*				  peer_port:		  12345
		*				  /monitor/:          enabled
		*				  /monitor_port/:     100
	    *				  /max_connections/:  *256*
		*				  /session/:		  *30s*	(enabled = 30s/disabled = 0s/..s/..m/..h/..d) Ignored for http(s) protocol.
		*				  /groups/:
		*						- Name refering to node_groups::group::name
		*						- ...
		*						
		*				  /nodes/:
		*						- protocol:			tcp (tcp/udp/http/https)
		*		 				  ip:				123.123.123.123
		*		 				  port:				12345
		*
		*						- ...
		*
		*				- ...
		*				
		*			/node_groups/:
		*				- group:
		*					- name: group1
		*					  nodes:
		*						  - Same fields as services::nodes
								  - ...
		*
		*/
		class Config
		{

		public:

			friend class Balancer;

			/**
			* @breif Default constructor.
			*
			*/
			Config();

			/**
			* @breif Copy constructor.
			*
			*/
			Config(const Config & config);

			/**
			* @breif Load configs from yaml file.
			*		 Missing configurations in the file will be set to default values.
			*
			* @param path Path of configuration file, including name.
			*
			* @throw dof::Exception on invalid file or configs.
			*
			*/
			virtual void LoadFromFile(const std::string & path);

			/**
			* @breif Save current configurations to file.
			*
			* @param filename Path of configuration file, including name.
			*
			* @return True if valid path, else false.
			*
			*/
			virtual bool SaveToFile(const std::string & filename);

		private:

			Yaml::Node m_ConfigRoot;

		};

		/**
		* @breif Default constructor.
		*
		*/
		Balancer();

		/**
		* @breif Destructor.
		*
		*/
		~Balancer();

		/**
		* @breif Get inter-process communication port.
		*
		*/
		unsigned short GetInterprocessPort() const;

		/**
		* @breif Get max concurrent connections of Doforward server.
		*
		*/
		unsigned short GetMaxConnections() const;

		/**
		* @breif 
		*
		* @param config Reference to configuration class.
		*				If config is uninitialized(LoadFromFile has not been called or succeeded),
		*				default configurations are used.
		*
		* @see Config for default configurations.
		*
		* @throw dof::Exception if initialization fails.
		*
		*/
		void Run(Config & config);

		/**
		* @breif Stop the server.
		*
		* @see Finish
		*
		*/
		void Stop();

		/**
		* @breif Function call to Finish() will lock the thread until Stop() is being called.
		*
		* @see Stop
		*
		*/
		void Finish();

		/**
		* @breif Add new service to balancer.
		*		 The balancer takes full responsibility over the service.
		*
		* @return True if added, false if already added.
		*
		*/
		bool AddService(Service & service);

		/**
		* @breif Remove service from balancer.
		*		 The service will be destroyed and deallocated.
		*
		* @return True if removed, false if service is not found.
		*
		*/
		bool RemoveService(Service & service);

		/**
		* @breif Get service.
		*
		* @param name		Name of service.
		* @param port		Port of service.
		* @param protocol	Protocol of serivce.
		*
		* @return Refernce to found service. If not found, InvalidService is returned.
		*
		*/
		Service & GetService(const std::string & name);
		Service & GetService(const unsigned short port, const Network::Protocol::eType protocol);


		/**
		* @breif Add new node to balancer and associate with service.
		*		 The balancer takes full responsibility over the node.
		*
		* @param node Reference to node to add.
		* @param service Reference to service to associate node with.
		*
		* @return True if added, false if already added.
		*
		*/
		bool AddNode(Node & node, Service & service);

		/**
		* @breif Remove node from balancer and detatch from service.
		*		 The service will be destroyed and deallocated.
		*
		* @return True if removed, false if service is not found.
		*
		*/
		bool RemoveNode(Node & node);

		/**
		* @breif Get node.
		*
		* @param name		Name of node.
		* @param address	Ip address of node.
		* @param port		Port of node.
		* @param protocol	Protocol of node.
		*
		* @return Refernce to found node. If not found, InvalidNode is returned.
		*
		*/
		Node & GetNode(const std::string & name);
		Node & GetNode(const std::string & address, const unsigned short port, const Network::Protocol::eType protocol);

		/**
		* @breif Invlaid types to compare return results with.
		*
		*/
		static const Service	InvalidService;
		static const Node		InvalidNode;

	private:

		/**
		* @breif Copy constructor.
		*
		*/
		Balancer(const Balancer & balancer);

		/**
		* @breif Load configurations.
		*
		*/
		void LoadConfig(Config & config);

		/**
		* @breif Helper function of LoadConfig, loads one service from yaml config.
		*
		*/
		void LoadConfigService(Yaml::Node & service, const unsigned int index);

		/**
		* @breif Helper function of LoadConfigService, loads one node from yaml config.
		*
		*/
		void LoadConfigNode(Yaml::Node & node, Service & service, const unsigned int index, const unsigned int service_index);

		/**
		* @breif Get the next service name available.
		*
		*/
		std::string GetNextServiceName();

		/**
		* @breif Get the next node name available.
		*
		*/
		std::string GetNextNodeName();

		typedef std::pair<unsigned short, Network::Protocol::eType>					PortProtocolPair;
		typedef std::tuple<std::string, unsigned short, Network::Protocol::eType>	AddressPortProtocolPair;

		unsigned short									m_InterprocessPort;			///< Port for inter-process communication with Doforward server.
		unsigned int									m_MaxConnections;			///< Max concurrent connections.
		
		Safe<std::set<Service *>>						m_Services;					///< Set of services
		Safe<std::map<std::string, Service *>>			m_ServicesName;				///< Map of services. Name as key.
		Safe<std::map<PortProtocolPair, Service *>>		m_ServicesPortProto;		///< Map of services. Port and protocol as key.
			
		Safe<std::set<Node *>>							m_Nodes;					///< Set of nodes.
		Safe<std::map<std::string, Node *>>				m_NodesName;				///< Map of nodes. Name as key.
		Safe<std::map<AddressPortProtocolPair, Node *>>	m_NodesAddressPortProto;	///< Map of nodes. Address, Port and protocol as key.

		
	};

}