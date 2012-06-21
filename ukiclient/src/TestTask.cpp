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

	DLOG(INFO) << "Test hash";
	int r = database->hash_create("test1");
	DLOG(INFO) << "r =" << r;

	r = database->hash_set("test1", "1", "a");
	DLOG(INFO) << "r =" << r;
	r = database->hash_set("test1", "2", "b");
	DLOG(INFO) << "r =" << r;
	uint32_t hsize;
	r = database->hash_size("test1", hsize);
	DLOG(INFO) << "r =" << r << ", size = " << hsize;

	string hval;
	r = database->hash_get("test1", "1", hval);
	DLOG(INFO) << "r =" << r << ", val = " << hval;
	r = database->hash_del("test1", "1");
	DLOG(INFO) << "r =" << r;
	r = database->hash_size("test1", hsize);
	DLOG(INFO) << "r =" << r << ", size = " << hsize;

	r = database->hash_clear("test1");
	DLOG(INFO) << "r =" << r;
	r = database->hash_size("test1", hsize);
	DLOG(INFO) << "r =" << r << ", size = " << hsize;
	r = database->hash_drop("test1");
	DLOG(INFO) << "r =" << r;

//	uint64_t type = database->next_type_id();
//	DLOG(INFO) << "type =" << type;
//
//	uint64_t val;
//	int r = database->seq_create("test1", 10);
//	DLOG(INFO) << "r =" << r;
//	r = database->seq_next("test1", val);
//	DLOG(INFO) << "r =" << r << ", val=" << val;
//	r = database->seq_next("test1", val);
//	DLOG(INFO) << "r =" << r << ", val=" << val;
//	r = database->seq_drop("test1");
//	DLOG(INFO) << "r =" << r;
//
//	//desc
//	DLOG(INFO) << "Test sequence";
//	r = database->seq_create("test1", 1000, false);
//	DLOG(INFO) << "r =" << r;
//	r = database->seq_next("test1", val);
//	DLOG(INFO) << "r =" << r << ", val=" << val;
//	r = database->seq_next("test1", val);
//	DLOG(INFO) << "r =" << r << ", val=" << val;
//	r = database->seq_drop("test1");
//	DLOG(INFO) << "r =" << r;
//
//	DLOG(INFO) << "Test collection";
//	uint64_t cid;
//	r = database->col_create("test5", cid);
//	DLOG(INFO) << "r =" << r << ", cid=" << cid;
//
//	r = database->col_put(cid, "a1", "b1");
//	DLOG(INFO) << "r =" << r;
//	r = database->col_put(cid, "a1", "b1");
//	DLOG(INFO) << "r =" << r;
//
//	string cval;
//	r = database->col_get(cid, "a1", cval);
//	DLOG(INFO) << "r =" << r << ", val=" << cval;
//	r = database->col_del(cid, "a1");
//	DLOG(INFO) << "r =" << r;
//	r = database->col_get(cid, "a1", cval);
//	DLOG(INFO) << "r =" << r << ", val=" << cval;
//
//	r = database->col_drop("test5");
//	DLOG(INFO) << "r =" << r;
//
//	DLOG(INFO) << "Test list";
//	r = database->list_create("test5");
//	DLOG(INFO) << "r =" << r;
//
//	r = database->list_push_back("test5", "aaaa");
//	DLOG(INFO) << "r =" << r;
//
//	r = database->list_push_front("test5", "bbbb");
//	DLOG(INFO) << "r =" << r;
//
//	r = database->list_push_front("test5", "cccc");
//	DLOG(INFO) << "r =" << r;
//
//
//	uint32_t lsize = 6;
//	r = database->list_size("test5", lsize);
//	DLOG(INFO) << "r =" << r << ", size =" << lsize;
//
//	for (int i = 0; i < lsize; i++)
//	{
//
//		string val;
//		r = database->list_index("test5", i, val);
//		DLOG(INFO) << "r =" << r << ", val["<<i<<"] = " << val;
//	}
//
//	r = database->list_clear("test5");
//	DLOG(INFO) << "r =" << r;
//
//	r = database->list_size("test5", lsize);
//	DLOG(INFO) << "r =" << r << ", size =" << lsize;
//
//	r = database->list_drop("test5");
//	DLOG(INFO) << "r =" << r;

	DLOG(INFO) << "Test task";
	return true;
}

} /* namespace ukibase */
