/*
 * main.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include <iostream>
#include <glog/logging.h>
#include "Engine.h"
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include "NodeConf.h"
#include "db/Database.h"

using namespace std;
using namespace ukicore;

int main(int argc, char *argv[])
{
	/* Initialize logger */
	google::InitGoogleLogging(argv[0]);
	google::LogToStderr();
	Engine& engine = Engine::get_instance();

	/* register components */
	component_ptr server = boost::make_shared<ukibase::NodeConf>();
	engine.register_component(server);

	component_ptr database = boost::make_shared<ukibase::Database>();
	engine.register_component(database);

	engine.init(argv[1]);
	engine.start();
	return 0;
}
