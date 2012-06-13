/*
 * ObjectService.cpp
 *
 *  Created on: Apr 19, 2012
 *      Author: hongsan
 */

#include "ObjectService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
using namespace std;

namespace ukibase
{

ObjectService::ObjectService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

ObjectService::~ObjectService()
{
}

bool ObjectService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::OBJ_GET:
		get(message, connection);
		return true;
	case MessageType::OBJ_SET:
		set(message, connection);
		return true;
	case MessageType::OBJ_DEL:
		del(message, connection);
		return true;
	case MessageType::OBJ_INC:
		inc(message, connection);
		return true;

	}
	return false;
}

void ObjectService::get(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, type;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_var32_(req, type);
	_dec_get_string_(req, key);

	if (!_dec_valid_(req))
	{
		_enc_declare_(rep, 32);
		_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
		_enc_put_var32_(rep, ErrorCode::BAD_FORMAT);
		_enc_update_msg_size_(rep);
		connection->send(_enc_data_(rep), _enc_size_(rep));
		return;
	}

	_enc_declare_(key, key.size() + 32);
	_enc_put_var32_(key, type);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	string val;
	int r = database->get(skey, &val);
	int size = val.size() + 64;
	_enc_declare_(rep, size);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, val);
	}
	_enc_update_msg_size_(rep);
	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SET for key " << key;
	}
}

void ObjectService::set(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, type;
	string key, val;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_var32_(req, type);
	_dec_get_string_(req, key);
	_dec_get_string_(req, val);

	if (!_dec_valid_(req))
	{
		_enc_declare_(rep, 32);
		_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
		_enc_put_var32_(rep, ErrorCode::BAD_FORMAT);
		_enc_update_msg_size_(rep);
		connection->send(_enc_data_(rep), _enc_size_(rep));
		return;
	}
	_enc_declare_(key, key.size() + 32);
	_enc_put_var32_(key, type);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = database->set(skey, val);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SET for key " << key;
	}
}

void ObjectService::del(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, type;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_var32_(req, type);
	_dec_get_string_(req, key);

	if (!_dec_valid_(req))
	{
		_enc_declare_(rep, 32);
		_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
		_enc_put_var32_(rep, ErrorCode::BAD_FORMAT);
		_enc_update_msg_size_(rep);
		connection->send(_enc_data_(rep), _enc_size_(rep));
		return;
	}

	_enc_declare_(key, key.size() + 32);
	_enc_put_var32_(key, type);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = database->del(skey);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

void ObjectService::inc(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard, type;
	string key;
	uint64_t val;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_var32_(req, type);
	_dec_get_string_(req, key);
	_dec_get_var64_(req, val);

	if (!_dec_valid_(req))
	{
		_enc_declare_(rep, 32);
		_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
		_enc_put_var32_(rep, ErrorCode::BAD_FORMAT);
		_enc_update_msg_size_(rep);
		connection->send(_enc_data_(rep), _enc_size_(rep));
		return;
	}

	_enc_declare_(key, key.size() + 32);
	_enc_put_var32_(key, type);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = database->inc(skey, val);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var64_(rep, val);
	}
	_enc_update_msg_size_(rep);
	connection->send(_enc_data_(rep), _enc_size_(rep));
}

} /* namespace db */
