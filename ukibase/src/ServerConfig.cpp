/*
 * Server.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include "ServerConfig.h"
#include "Engine.h"
#include "TaskConnect.h"

namespace ukibase
{

ServerConfig::ServerConfig() :
		Component("serverconf")
{
	nodes.set_deleted_key(-1);
	nodes.set_empty_key(-2);
}

ServerConfig::~ServerConfig()
{

}

void ServerConfig::init()
{
	Config& config = Engine::get_instance().get_configuration();
	Setting &sconf = config.getRoot()["general"];
	if (!sconf.lookupValue("man_host", man_host))
	{
		throw std::runtime_error("Configuration file do not contain man_host parameter");
	}

	if (!sconf.lookupValue("man_port", man_port))
	{
		throw std::runtime_error("Configuration file do not contain man_port parameter");
	}

	if (!sconf.lookupValue("server_id", server_id))
	{
		throw std::runtime_error("Configuration file do not contain server id parameter");
	}
}

void ServerConfig::start()
{
	task_ptr task = new TaskConnect(man_host, man_port);
	Engine::get_instance().get_tasks().enqueue(task);
}

void ServerConfig::stop()
{

}

} /* namespace ukibase */
