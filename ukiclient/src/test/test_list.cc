/*
 * test_list.cc
 *
 *  Created on: Jun 22, 2012
 *      Author: nhsan
 */

#include "gtest/gtest.h"
#include "../db/Database.h"
#include "Engine.h"
#include "glog/logging.h"
using namespace dbclient;

TEST(list, create_drop)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->list_create("test1");
	ASSERT_TRUE(r==0);

	r = database->list_create("test1");
	ASSERT_TRUE(r==ErrorCode::ITEM_EXISTED);

	r = database->list_drop("test1");
	ASSERT_TRUE(r==0);
}

TEST(list, push_clear)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->list_create("test1");
	ASSERT_TRUE(r==0);

	r = database->list_push_back("test1", "1");
	ASSERT_TRUE(r==0);
	r = database->list_push_front("test1", "2");
	ASSERT_TRUE(r==0);
	r = database->list_push_back("test1", "3");
	ASSERT_TRUE(r==0);
	uint32_t size;
	r = database->list_size("test1", size);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(size==3);

	r = database->list_clear("test1");
	ASSERT_TRUE(r==0);
	r = database->list_size("test1", size);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(size==0);

	r = database->list_drop("test1");
	ASSERT_TRUE(r==0);
}

TEST(list, index_remove)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->list_create("test1");
	ASSERT_TRUE(r==0);

	r = database->list_push_back("test1", "1");
	ASSERT_TRUE(r==0);
	r = database->list_push_front("test1", "2");
	ASSERT_TRUE(r==0);

	string val;

	r = database->list_index("test1", 0, val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="2");

	r = database->list_index("test1", 1, val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="1");

	r = database->list_index("test1", 2, val);
	ASSERT_TRUE(r==ErrorCode::IS_NOT_FOUND);

	r = database->list_remove("test1", 0);
	ASSERT_TRUE(r==0);

	r = database->list_index("test1", 0, val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="1");

	r = database->list_drop("test1");
	ASSERT_TRUE(r==0);
}

