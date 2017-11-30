#pragma once

#include <string>
#include <thread>

namespace dof
{

	/**
	* @breif Main class of DoForward server.
	*		 The balancer will load configurations, balance connections and packets,
	*		 handle disconnections, gather statistics, etc...
	*
	*/
	class Node
	{

	public:

		/**
		* @breif Configuration class of the balancer,
		*		 including specifying nodes, protocols, ports, etc...
		*
		*		 Default configurations
		*		 ---------------------------------------------------
		*			server:
		*				- max_connections: 256
		*			nodes:
		*				-
		*			api:
		*				- port: 12345
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
		Node();

		

	private:


	};

}
