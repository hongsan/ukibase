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
//#include "services/SequencerService.h"
//#include "services/ListService.h"
#include "commons/MessageUtil.h"
#include "../NodeConf.h"
#include "commons/MurmurHash3.h"

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
}

Database::~Database()
{
	delete options.block_cache;
	delete db;
}

uint64_t Database::next_type_id()
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	string id = SEQ_DATATYPE;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_TYPE, msg_id, shard);
	_enc_update_msg_size_(req);

	servernode_ptr node = ring->get_node(shard);
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));
	engine.save_context(context.get());
	node->connection->send(_enc_data_(req), _enc_size_(req));
	context->wait();
	engine.release_context(context.get());
	if (!context->done) return 0;

	int code;
	_dec_declare2_(rep, context->get_reply()->get_content_data(), context->get_reply()->get_content_size());
	_dec_get_var32_(rep, code);
	uint64_t val;
	if (code == ErrorCode::OK)
	{
		_dec_get_var64_(rep, val);
	}
	if (!_dec_valid_(rep)) return 0;
	if (code != ErrorCode::OK) return 0;
	return val;
}

void Database::init()
{
	NodeConf* config = (NodeConf*) Engine::get_instance().get_component(COMP_SERVERCONF).get();
	ring = config->ring;
	me = config->me;

//	DLOG(INFO) << "Register ObjectService...";
//	REGISTER_SERVICE (ObjectService);
//	DLOG(INFO)<<"Register SequencerService...";
//	REGISTER_SERVICE(SequencerService);
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

int Database::inc(leveldb::Slice& key, uint64_t& val)
{
	string sval;
	leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &sval);
	val = 0;
	if (status.ok())
	{
		_dec_declare2_(v, sval.c_str(), sval.size());
		_dec_get_fix64_(v, val);
		if (!_dec_valid_(v)) return ErrorCode::DB_ERROR;
	}
	val++;
	_enc_declare_(res, 32);
	_enc_put_fix64_(res, val);
	string result(_enc_data_(res), _enc_size_(res));
	status = db->Put(leveldb::WriteOptions(), key, result);
	if (status.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

} /* namespace db */
