/*
 * ListService.cpp
 *
 *  Created on: May 13, 2012
 *      Author: hongsan
 */

#include "ListService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
using namespace std;

namespace ukibase
{

ListService::ListService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

ListService::~ListService()
{
}

bool ListService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::LIST_CREATE:
		create(message, connection);
		return true;
	case MessageType::LIST_DESTROY:
		destroy(message, connection);
		return true;
	case MessageType::LIST_PUSHBACK:
		push_back(message, connection);
		return true;
	case MessageType::LIST_PUSHFRONT:
		push_front(message, connection);
		return true;
	case MessageType::LIST_SIZE:
		size(message, connection);
		return true;
	case MessageType::LIST_INDEX:
		index(message, connection);
		return true;
	case MessageType::LIST_REMOVE:
		remove(message, connection);
		return true;
	case MessageType::LIST_RANGE:
		range(message, connection);
		return true;
	case MessageType::LIST_CLEAR:
		clear(message, connection);
		return true;
	}
	return false;
}

void ListService::create(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	int r = database->list_create(key);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::destroy(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	int r = database->list_clear(key);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::clear(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	int r = database->list_clear(key);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::push_back(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key, val;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_string_(req, val);

	DECODE_CHECK;

	int r = database->list_pushback(key, val);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::push_front(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key, val;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_string_(req, val);

	DECODE_CHECK;

	int r = database->list_pushfront(key, val);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::size(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	uint32_t size;
	int r = database->list_size(key, size);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var32_(rep, size);
	}
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::index(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, index;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_var32_(req, index);

	DECODE_CHECK;

	string val;
	int r = database->list_index(key, index, val);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, val);
	}
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::remove(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, index;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_var32_(req, index);

	DECODE_CHECK;

	string val;
	int r = database->list_remove(key, index);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ListService::range(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, index, count;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_var32_(req, index);
	_dec_get_var32_(req, count);

	DECODE_CHECK;

	uint32_t data_size = MAXPACKETSIZE;
	uint32_t c = count;

	_enc_declare_(rep, 32 + data_size);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	int r = database->list_range(key, index, c, (char*)(_enc_data_(rep) + _enc_size_(rep) + 8), data_size);

	if (r == ErrorCode::OK)
	{
		_enc_put_fix32_(rep, r);
		_enc_put_fix32_(rep, c);
		_enc_size_(rep) += data_size;
	}
	else
	{
		_enc_put_fix32_(rep, r);
	}
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

} /* namespace core */
