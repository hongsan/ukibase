/*
 * test_sequence.cc
 *
 *  Created on: Jun 22, 2012
 *      Author: nhsan
 */

#include "gtest/gtest.h"
#include "../db/Database.h"
#include "Engine.h"
#include "glog/logging.h"
using namespace dbclient;

TEST(sequence, seq_create_drop)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->seq_create("test1", 10);
	ASSERT_TRUE(r==0);

	r = database->seq_create("test1", 10);
	ASSERT_TRUE(r==ErrorCode::ITEM_EXISTED);

	r = database->seq_drop("test1");
	ASSERT_TRUE(r==0);

	r = database->seq_drop("test1");
	ASSERT_TRUE(r==ErrorCode::IS_NOT_FOUND);
}

TEST(sequence, seq_next1)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->seq_create("test1", 10);
	ASSERT_TRUE(r==0);

	uint64_t val;

	for (int i = 0; i < 100; i++)
	{
		r = database->seq_next("test1", val);
		ASSERT_TRUE(r==0);
		ASSERT_TRUE(val== (i+11));
	}

	r = database->seq_drop("test1");
	ASSERT_TRUE(r==0);
}

TEST(sequence, seq_next2)
{
	Database* database = (Database*) Engine::get_instance().get_component(COMP_DBCLIENT).get();
	ASSERT_TRUE(database != NULL);

	int r = database->seq_create("test1", 1000, false);
	ASSERT_TRUE(r==0);

	uint64_t val;

	for (int i = 0; i < 100; i++)
	{
		r = database->seq_next("test1", val);
		ASSERT_TRUE(r==0);
		ASSERT_TRUE(val== (1000-i-1));
	}

	r = database->seq_drop("test1");
	ASSERT_TRUE(r==0);
}
