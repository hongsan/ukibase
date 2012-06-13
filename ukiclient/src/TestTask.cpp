/*
 * TestTask.cpp
 *
 *  Created on: Jun 8, 2012
 *      Author: nhsan
 */

#include "TestTask.h"
#include "db/Database.h"
#include "Engine.h"
#include "commons/Constant.h"
#include <glog/logging.h>
namespace dbclient
{

TestTask::TestTask()
{
}

TestTask::~TestTask()
{
}

bool TestTask::execute(Worker* worker)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	if (database == NULL)
	{
		DLOG(INFO) << "Error in get database";
	}

	DLOG(INFO) << "Test task";
	return true;
}

} /* namespace ukibase */
