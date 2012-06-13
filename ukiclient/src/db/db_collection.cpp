/*
 * db_collection.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: nhsan
 */

#include "Database.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"

namespace dbclient
{

int Database::col_create(string id, uint64_t& cid)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::COLLECTION, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_CREATE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_var64_(rep, cid);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_drop(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::COLLECTION, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_DROP, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_id(string id, uint64_t& cid)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::COLLECTION, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_ID, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_var64_(rep, cid);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_get(uint64_t cid, string key, string& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), cid, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_GET, msg_id, 0);
	_enc_put_var64_(req, cid);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;
	if (code == ErrorCode::OK)
	{
		_dec_get_string_(rep, val);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_put(uint64_t cid, string key, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), cid, &shard);

	int size = key.size() + val.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_PUT, msg_id, 0);
	_enc_put_var64_(req, cid);
	_enc_put_string_(req, key);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_del(uint64_t cid, string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), cid, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_DEL, msg_id, 0);
	_enc_put_var64_(req, cid);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::col_exist(uint64_t cid, string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), cid, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::COL_EXIST, msg_id, 0);
	_enc_put_var64_(req, cid);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

}
