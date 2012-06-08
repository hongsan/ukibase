/*
 * Database.cpp
 *
 *  Created on: Apr 13, 2012
 *      Author: hongsan
 */

#include "Database.h"
#include "leveldb/cache.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "services/SequencerService.h"
#include "services/ListService.h"
#include "commons/MessageUtil.h"
#include "../NodeConf.h"

namespace ukibase
{

Database::Database() :
		Component(COMP_DBSERVER)
{
	options.block_cache = leveldb::NewLRUCache(100 * 1048576); // 100MB cache
	options.write_buffer_size = 50 * 1048576;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, "db", &db);
	assert(status.ok());

	locks.set_empty_key("");
	locks.set_deleted_key(" ");
}

Database::~Database()
{
	delete options.block_cache;
	delete db;
}

void Database::init()
{
	NodeConf* config = (NodeConf*) Engine::get_instance().get_component(COMP_SERVERCONF).get();
	ring = config->ring;
	me = config->me;
	DLOG(INFO)<<"Register ListService...";
	REGISTER_SERVICE(ListService);
	DLOG(INFO)<<"Register SequencerService...";
	REGISTER_SERVICE(SequencerService);
}

int Database::set(leveldb::Slice& key, string& val)
{
	leveldb::Status status = db->Put(leveldb::WriteOptions(), key, val);
	if (status.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

int Database::get(leveldb::Slice& key, string* val)
{
	leveldb::Status status = db->Get(leveldb::ReadOptions(), key, val);
	if (status.ok()) return ErrorCode::OK;
	if (status.IsNotFound()) return ErrorCode::IS_NOT_FOUND;
	return ErrorCode::DB_ERROR;
}

int Database::del(leveldb::Slice& key)
{
	leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
	if (status.ok()) return ErrorCode::OK;
	if (status.IsNotFound()) return ErrorCode::IS_NOT_FOUND;
	return ErrorCode::DB_ERROR;
}

bool Database::exist(leveldb::Slice& key)
{
	string val;
	leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &val);
	if (status.ok()) return true;
	if (status.IsNotFound()) return false;
	return false;
}

} /* namespace db */
