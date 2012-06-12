/*
 * Server.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include "NodeConf.h"
#include "Engine.h"
#include "TaskConnect.h"
#include <glog/logging.h>
#include "commons/Constant.h"

namespace dbclient
{

NodeConf::NodeConf() : Component(COMP_SERVERCONF)
{
	nodes.set_deleted_key(-1);
	nodes.set_empty_key(-2);
	ring = boost::make_shared<HashRing>(NUMBER_REPLICAS);
}

NodeConf::~NodeConf()
{
}

void NodeConf::init()
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
}

void NodeConf::start()
{
	task_ptr task = new TaskConnect(man_host, man_port);
	Engine::get_instance().get_tasks().enqueue(task);
}

void NodeConf::stop()
{

}

} /* namespace ukibase */
