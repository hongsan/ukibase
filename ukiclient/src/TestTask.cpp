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
		DLOG(INFO)<<"Error in get database";
	}

	int r = database->list_create("abc");
	DLOG(INFO) << "r=" << r;
	r = database->list_push_back("abc", "def1");
	DLOG(INFO) << "r=" << r;
	r = database->list_push_front("abc", "def2");
	DLOG(INFO) << "r=" << r;
	uint32_t size;
	r = database->list_size("abc", size);
	DLOG(INFO) << "r=" << r << " size = " << size;

	string val;
	r = database->list_index("abc", 0, val);
	DLOG(INFO) << "r=" << r << " val0 = " << val;

	r = database->list_index("abc", 1, val);
	DLOG(INFO) << "r=" << r << " val1 = " << val;

	vector<string> tmp;
	r = database->list_range("abc", 1, 3, tmp);
	DLOG(INFO) << "r=" << r << " size = " << tmp.size();
	for (int i = 0; i < tmp.size(); i++)
	{
		DLOG(INFO) << "v[" << i << "] =" << tmp[i];
	}

	r = database->list_remove("abc", 0);
	DLOG(INFO) << "r=" << r;

	r = database->list_size("abc", size);
	DLOG(INFO) << "r=" << r << " size = " << size;

	r = database->list_index("abc", 1, val);
	DLOG(INFO) << "r=" << r << " val0 = " << val;

	r = database->list_destroy("abc");
	DLOG(INFO) << "r=" << r;

	DLOG(INFO)<<"Test task";
	return true;
}

} /* namespace ukibase */
