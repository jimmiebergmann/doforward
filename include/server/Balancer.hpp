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

#include <string>
#include <Node.hpp>
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
		*				- name:				service1
		*				  protocol:			tcp (tcp/udp/http/https)
		*				  peer_port:		12345
		*				  node_port:		54321
		*				  max_connections:	*256*
		*				  /session/:		*30s*	(enabled = 30s/disabled = 0s/..s/..m/..h/..d) Ignored for http(s) protocol.
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
			virtual bool SaveToFile(const std::string & path);

		private:

			// ...

		};

		/**
		* @breif Default constructor.
		*
		*/
		Balancer();

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

	private:

		/**
		* @breif Copy constructor.
		*
		*/
		Balancer(const Balancer & balancer);

		unsigned short					m_InterprocessPort; ///< Port for inter-process communication with Doforward server.
		unsigned int					m_MaxConnections;	///< Max concurrent connections.
		std::map<std::string, Node *>	m_Nodes;			///< Map of nodes.

	};

}
