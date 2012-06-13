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

	int r = database->set(1, "abc", "def");
	DLOG(INFO) << "r=" << r;
	string val;
	database->get(1, "abc", &val);
	DLOG(INFO) << "r=" << r << " val=" << val;

	uint64_t ival = 1;
	database->inc(1, "aaa", ival);
	DLOG(INFO) << "r=" << r << " val=" << ival;
	ival = 1;
	database->inc(1, "aaa", ival);
	DLOG(INFO) << "r=" << r << " val=" << ival;

	DLOG(INFO) << "Test task";
	return true;
}

} /* namespace ukibase */
