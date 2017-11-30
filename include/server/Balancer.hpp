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
	*/
	class Balancer
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
		Balancer();

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

		std::map<std::string, Node *> m_Nodes; ///< Map of 

	};

}
