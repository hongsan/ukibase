/*
 * db_sequence.cpp
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
int Database::seq_create(string id, uint64_t init, bool asc)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_CREATE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_put_var64_(req, init);
	if (asc)
	{
		_enc_put_byte_(req, 1);
	}
	else
	{
		_enc_put_byte_(req, 0);
	}
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::seq_drop(string id)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_DROP, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::seq_next(string id, uint64_t& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_NEXT, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;
	if (code == ErrorCode::OK)
	{
		_dec_get_var64_(rep, val);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

}

