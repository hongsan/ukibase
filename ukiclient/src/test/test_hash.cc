/*
 * test_hash.cc
 *
 *  Created on: Jun 22, 2012
 *      Author: nhsan
 */

#include "gtest/gtest.h"
#include "../db/Database.h"
#include "Engine.h"
#include "glog/logging.h"
using namespace dbclient;


TEST(hash, create_drop)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->hash_create("test1");
	ASSERT_TRUE(r==0);

	r = database->hash_create("test1");
	ASSERT_TRUE(r==ErrorCode::ITEM_EXISTED);

	r = database->hash_drop("test1");
	ASSERT_TRUE(r==0);
}

TEST(hash, get_set_del)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->hash_create("test1");
	ASSERT_TRUE(r==0);

	r = database->hash_set("test1","1","a");
	ASSERT_TRUE(r==0);
	r = database->hash_set("test1","2","b");
	ASSERT_TRUE(r==0);
	r = database->hash_set("test1","2","c");
	ASSERT_TRUE(r==0);

	uint32_t size;
	r = database->hash_size("test1",size);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(size==2);

	string val;
	r = database->hash_get("test1","1",val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="a");
	r = database->hash_get("test1","2",val);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(val=="c");

	r = database->hash_clear("test1");
	ASSERT_TRUE(r==0);
	r = database->hash_size("test1",size);
	ASSERT_TRUE(r==0);
	ASSERT_TRUE(size==0);


	r = database->hash_drop("test1");
	ASSERT_TRUE(r==0);
}
