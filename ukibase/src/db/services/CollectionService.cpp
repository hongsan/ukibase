/*
 * CollectionService.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: nhsan
 */

#include "CollectionService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
using namespace std;

namespace ukibase
{

CollectionService::CollectionService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

CollectionService::~CollectionService()
{
}

bool CollectionService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::COL_CREATE:
		create(message, connection);
		return true;
	case MessageType::COL_DROP:
		drop(message, connection);
		return true;
	case MessageType::COL_ID:
		id(message, connection);
		return true;
	case MessageType::COL_GET:
		get(message, connection);
		return true;
	case MessageType::COL_PUT:
		put(message, connection);
		return true;
	case MessageType::COL_DEL:
		del(message, connection);
		return true;
	case MessageType::COL_EXIST:
		exist(message, connection);
		return true;
	}
	return false;
}

void CollectionService::del(message_ptr const & message, connection_ptr const & connection)
{
	uint64_t cid;
	string key;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_var64_(req, cid);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	_enc_declare_(key, key.size() + 32);
	_enc_put_var64_(key, cid);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));
	int r = database->del(skey);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_DEL for key " << key;
	}
}


void CollectionService::put(message_ptr const & message, connection_ptr const & connection)
{
	uint64_t cid;
	string key, val;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_var64_(req, cid);
	_dec_get_string_(req, key);
	_dec_get_string_(req, val);

	DECODE_CHECK;

	_enc_declare_(key, key.size() + 32);
	_enc_put_var64_(key, cid);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));
	int r = database->set(skey, val);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_PUT for key " << key;
	}
}

void CollectionService::get(message_ptr const & message, connection_ptr const & connection)
{
	uint64_t cid;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_var64_(req, cid);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	_enc_declare_(key, key.size() + 32);
	_enc_put_var64_(key, cid);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));
	string sval;
	int r = database->get(skey, &sval);

	_enc_declare_(rep, (sval.size()+32));
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, sval);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_GET for key " << key;
	}
}

void CollectionService::exist(message_ptr const & message, connection_ptr const & connection)
{
	uint64_t cid;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_var64_(req, cid);
	_dec_get_string_(req, key);

	DECODE_CHECK;

	_enc_declare_(key, key.size() + 32);
	_enc_put_var64_(key, cid);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

	int r = ErrorCode::IS_NOT_FOUND;
	if (database->exist(skey)) r = ErrorCode::OK;

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_EXIST for key " << key;
	}
}

void CollectionService::id(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::COLLECTION);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

	string sid;
	uint64_t cid;
	int r = database->get(skey, &sid);

	if (r == ErrorCode::OK)
	{
		_dec_declare1_(id, sid);
		_dec_get_fix64_(id, cid);
		if (!_dec_valid_(id)) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);

	if (r == ErrorCode::OK)
	{
		_enc_put_var64_(rep, cid);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_ID for key " << id;
	}
}

void CollectionService::create(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::COLLECTION);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

	int r = ErrorCode::OK;
	uint64_t type_id = 0;
	if (!database->exist(skey))
	{
		type_id = database->next_type_id();

		//DLOG(INFO) << "typeid=" << type_id;
		if (type_id > 0)
		{
			_enc_declare_(col, 64);
			_enc_put_fix64_(col, type_id);
			string val2(_enc_data_ (col), _enc_size_ (col));
			r = database->set(skey, val2);
		}
		else r = ErrorCode::DB_ERROR;
	}
	else r = ErrorCode::ITEM_EXISTED;

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var64_(rep, type_id);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_CREATE for key " << id;
	}
}

void CollectionService::drop(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::COLLECTION);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

	int r = database->del(skey);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from COL_DROP for key " << id;
	}
}

} /* namespace ukibase */
