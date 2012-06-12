/*
 * Database.cpp
 *
 *  Created on: Apr 13, 2012
 *      Author: hongsan
 */

#include "Database.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"

namespace dbclient
{

Database::Database() :
		Component(COMP_DBCLIENT)
{
}

Database::~Database()
{

}

void Database::init()
{
	config = (NodeConf*) Engine::get_instance().get_component(COMP_SERVERCONF).get();
	assert(config != NULL);
}

int Database::set(uint32_t type, string key, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), type, &shard);

	int size = key.size() + val.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::OBJ_SET, msg_id, 0);
	_enc_put_fix32_(req, shard);
	_enc_put_var32_(req, type);
	_enc_put_string_(req, key);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::get(uint32_t type, string key, string* val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), type, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::OBJ_GET, msg_id, 0);
	_enc_put_fix32_(req, shard);
	_enc_put_var32_(req, type);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_string_(rep, (*val));
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::del(uint32_t type, string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), type, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::OBJ_DEL, msg_id, 0);
	_enc_put_fix32_(req, shard);
	_enc_put_var32_(req, type);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::exist(uint32_t type, string key)
{
	//TODO: implement
}

collection_ptr Database::get_collection(string id)
{

}

int Database::create_collection(string id)
{

}

int Database::drop_collection()
{

}

} /* namespace db */
