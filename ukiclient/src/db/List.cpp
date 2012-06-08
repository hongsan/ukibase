/*
 * List.cpp
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

using namespace ukicore;

namespace dbclient
{

int Database::list_create(string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	int size = key.size() + 64;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_CREATE, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_destroy(string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_DESTROY, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_push_back(string key, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	int size = key.size() + val.size() + 64;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_PUSHBACK, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_push_front(string key, string val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	int size = key.size() + val.size() + 64;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_PUSHFRONT, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_string_(req, val);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_mpush_back(string key, vector<string>& vals)
{

}

int Database::list_mpush_front(string key, vector<string>& vals)
{

}

int Database::list_size(string key, uint32_t& size)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, (key.size() + 64));
	_enc_put_msg_header_(req, MessageType::LIST_SIZE, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_var32_(rep, size);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_range(string key, int index, int count, vector<string>& vals)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, (key.size() + 64));
	_enc_put_msg_header_(req, MessageType::LIST_RANGE, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_var32_(req, index);
	_enc_put_var32_(req, count);
	_enc_update_msg_size_(req);

	servernode_ptr node = config->ring->get_node(shard);
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));
	engine.save_context(context.get());
	node->connection->send(_enc_data_(req), _enc_size_(req));
	context->wait();
	engine.release_context(context.get());
	if (!context->done) return ErrorCode::TIMEOUT;
	int code;
	_dec_declare2_(rep, context->get_reply()->get_content_data(), context->get_reply()->get_content_size());
	_dec_get_fix32_(rep, code);

	if (code == ErrorCode::OK)
	{
		int size;
		_dec_get_fix32_(rep, size);
		for (int i = 0; i < size; i++)
		{
			string v;
			_dec_get_string_(rep, v);
			vals.push_back(v);
		}
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_remove(string key, int index)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, (key.size() + 64));
	_enc_put_msg_header_(req, MessageType::LIST_REMOVE, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_var32_(req, index);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_index(string key, int index, string& val)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, (key.size() + 64));
	_enc_put_msg_header_(req, MessageType::LIST_INDEX, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_put_var32_(req, index);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (code == ErrorCode::OK)
	{
		_dec_get_string_(rep, val);
	}
	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

int Database::list_clear(string key)
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	MurmurHash3_x86_32(key.c_str(), key.size(), KeyType::LIST, &shard);

	int size = key.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::LIST_CLEAR, msg_id, shard);
	_enc_put_fix32_(req, shard);
	_enc_put_string_(req, key);
	_enc_update_msg_size_(req);

	DO_REQUEST_REPLY;

	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
	return code;
}

} // namespace dbclient

