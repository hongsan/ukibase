/*
 * Server.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include "NodeConf.h"
#include "Engine.h"
#include "TaskConnect.h"
#include "services/ConnectService.h"
#include <glog/logging.h>
#include "commons/Constant.h"

namespace ukibase
{

NodeConf::NodeConf() :
		Component(COMP_SERVERCONF)
{
	nodes.set_deleted_key(-1);
	nodes.set_empty_key(-2);
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

	if (!sconf.lookupValue("server_id", server_id))
	{
		throw std::runtime_error("Configuration file do not contain server id parameter");
	}

	/* register services */
	DLOG(INFO)<<"Register ConnectService...";
	service_ptr connect_service = boost::make_shared<ConnectService>();
	Engine::get_instance().register_service(connect_service);
	connect_service->activate();
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
