/*
 * db_hash.cpp
 *
 *  Created on: Jun 21, 2012
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

int Database::hash_create(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_CREATE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_drop(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_DROP, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_size(string id, uint32_t& size)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, id.size() + 32);
	_enc_put_msg_header_(req, MessageType::HASH_SIZE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_var32_(rep, size);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_clear(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_CLEAR, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_set(string id, string key, string value)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size() + key.size() + value.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_SET, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, key);
	_enc_put_string_(req, value);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_get(string id, string key, string& value)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size() + key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_GET, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;
	if (code == ErrorCode::OK)
	{
		_dec_get_string_(rep, value);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_del(string id, string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size()+key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_DEL, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::hash_exist(string id, string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::HASH, &shard);

	int size = id.size()+key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::HASH_EXIST, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

}

