/*
 * Task.cpp
 *
 *  Created on: Jun 3, 2012
 *      Author: nhsan
 */

#include "Task.h"
#include "Engine.h"
#include <glog/logging.h>
#include "commons/MessageUtil.h"
#include "commons/Constant.h"

namespace ukicore
{
Task::Task()
{
}

Task::~Task()
{
}

TaskRead::TaskRead(Connection* connection)
{
	this->connection = connection;
}

bool TaskRead::execute(Worker* worker)
{
	if (!connection->read_message(worker))
	{
		//TODO: handle connection error here
		Engine::get_instance().close_connection(connection->get_socket());
	}
	return false;
}

}
