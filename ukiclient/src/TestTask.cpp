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

	uint64_t type = database->next_type_id();
	DLOG(INFO)<<"type ="<<type;

	uint64_t val;
	int r = database->seq_create("test1", 10);
	DLOG(INFO)<<"r ="<<r;
	r = database->seq_next("test1", val);
	DLOG(INFO)<<"r ="<<r<<", val="<<val;
	r = database->seq_next("test1", val);
	DLOG(INFO)<<"r ="<<r<<", val="<<val;
	r = database->seq_drop("test1");
	DLOG(INFO)<<"r ="<<r;

	//desc
	r = database->seq_create("test1", 1000, false);
	DLOG(INFO)<<"r ="<<r;
	r = database->seq_next("test1", val);
	DLOG(INFO)<<"r ="<<r<<", val="<<val;
	r = database->seq_next("test1", val);
	DLOG(INFO)<<"r ="<<r<<", val="<<val;
	r = database->seq_drop("test1");
	DLOG(INFO)<<"r ="<<r;

	uint64_t cid;
	r=database->col_create("test5", cid);
	DLOG(INFO)<<"r ="<<r<<", cid="<<cid;

	r=database->col_put(cid, "a1", "b1");
	DLOG(INFO)<<"r ="<<r;
	r=database->col_put(cid, "a1", "b1");
	DLOG(INFO)<<"r ="<<r;

	string cval;
	r=database->col_get(cid, "a1", cval);
	DLOG(INFO)<<"r ="<<r<<", val="<<cval;
	r=database->col_del(cid, "a1");
	DLOG(INFO)<<"r ="<<r;
	r=database->col_get(cid, "a1", cval);
	DLOG(INFO)<<"r ="<<r<<", val="<<cval;

	r= database->col_drop("test5");
	DLOG(INFO)<<"r ="<<r;

	DLOG(INFO) << "Test task";
	return true;
}

} /* namespace ukibase */
