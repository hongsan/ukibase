/*
 * Sequencer.cpp
 *
 *  Created on: Apr 19, 2012
 *      Author: hongsan
 */

#include "Database.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"
#include "commons/HashUtil.h"

namespace dbclient
{

int Database::seq_create(string key, uint64_t initial)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::SEQ, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_CREATE, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_fix64_(req, initial);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::seq_destroy(string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::SEQ, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_DESTROY, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::seq_inc(string key, uint64_t& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::SEQ, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_INC, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_fix64_(rep, val);
	}

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::seq_dec(string key, uint64_t& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::SEQ, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_DEC, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_fix64_(rep, val);
	}

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

} // namespace dbclient

