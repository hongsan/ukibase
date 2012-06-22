/*
 * test_collection.cc
 *
 *  Created on: Jun 22, 2012
 *      Author: nhsan
 */
#include "gtest/gtest.h"
#include "../db/Database.h"
#include "Engine.h"
#include "glog/logging.h"
using namespace dbclient;

TEST(collection, col_create_drop)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	uint64_t cid;
	int r = database->col_create("test1", cid);
	ASSERT_TRUE(r==0);

	r = database->col_create("test1", cid);
	ASSERT_TRUE(r==ErrorCode::ITEM_EXISTED);

	r = database->col_drop("test1");
	ASSERT_TRUE(r==0);
}

TEST(collection, col_get_set_del)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	uint64_t cid;
	int r = database->col_create("test1", cid);
	ASSERT_TRUE(r==0);

	r = database->col_put(cid, "1","a");
	ASSERT_TRUE(r==0);

	r = database->col_exist(cid, "1");
	ASSERT_TRUE(r==0);

	string val;
	r = database->col_get(cid, "1",val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="a");

	r = database->col_del(cid, "1");
	ASSERT_TRUE(r==0);

	r = database->col_exist(cid, "1");
	ASSERT_TRUE(r==ErrorCode::IS_NOT_FOUND);

	r = database->col_drop("test1");
	ASSERT_TRUE(r==0);
}
