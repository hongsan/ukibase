/*
 * db_list.cpp
 *
 *  Created on: Jun 14, 2012
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

int Database::list_create(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_CREATE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_drop(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_DROP, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_size(string id, uint32_t& size)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, id.size() + 32);
	_enc_put_msg_header_(req, MessageType::LIST_SIZE, msg_id, shard);
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

int Database::list_clear(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_CLEAR, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_push_back(string id, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_PUSH_BACK, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_push_front(string id, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_PUSH_FRONT, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_index(string id, uint32_t index, string& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_INDEX, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_var32_(req, index);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code==ErrorCode::OK)
	{
		_dec_get_string_(rep, val);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_remove(string id, uint32_t index)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::LIST, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_REMOVE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_var32_(req, index);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_range(string id, uint32_t index, uint32_t count, vector<string>& vals)
{
}

}
