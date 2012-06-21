/*
 * HashService.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: nhsan
 */

#include "HashService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
#include "leveldb/write_batch.h"
using namespace std;

#define ENCODE_HASH_KEY(skey)\
_enc_declare_(key, id.size() + 32);\
_enc_put_var64_(key, METADATA_TYPE);\
_enc_put_var64_(key, DataType::HASH);\
_enc_put_buffer_(key, id, id.size());\
leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

#define HASH_GET_INFO \
ENCODE_HASH_KEY(skey);\
uint64_t hid;\
uint32_t count;\
string sval;\
int r = database->get(skey, &sval);\
if (r == ErrorCode::OK)\
{\
	_dec_declare1_(info, sval);\
	_dec_get_fix64_(info, hid);\
	_dec_get_var32_(info, count);\
	if (!_dec_valid_(info)) r = ErrorCode::DB_ERROR;\
}

#define ENCODE_ITEM_KEY(hid,key, out)\
_enc_declare_(k, 64);\
_enc_put_var64_(k, hid);\
_enc_append_key_(k, key);\
leveldb::Slice out(_enc_data_(k), _enc_size_(k));

#define ENCODE_HASH_INFO(info)\
_enc_declare_(info, 64);\
_enc_put_fix64_(info, hid);\
_enc_put_var32_(info, count);\
leveldb::Slice info(_enc_data_(info), _enc_size_(info));

namespace ukibase
{

HashService::HashService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

HashService::~HashService()
{

}

bool HashService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::HASH_CREATE:
		create(message, connection);
		return true;
	case MessageType::HASH_DROP:
		drop(message, connection);
		return true;
	case MessageType::HASH_CLEAR:
		clear(message, connection);
		return true;
	case MessageType::HASH_SIZE:
		size(message, connection);
		return true;
	case MessageType::HASH_SET:
		set(message, connection);
		return true;
	case MessageType::HASH_GET:
		get(message, connection);
		return true;
	case MessageType::HASH_DEL:
		del(message, connection);
		return true;
	case MessageType::HASH_EXIST:
		exist(message, connection);
		return true;
	}
	return false;
}

void HashService::create(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	ENCODE_HASH_KEY(skey);

	int r = ErrorCode::OK;
	uint64_t type_id = 0;
	if (!database->exist(skey))
	{
		type_id = database->next_type_id();
		//DLOG(INFO)<<"type="<<type_id;
		if (type_id > 0)
		{
			_enc_declare_(lst, 64);
			_enc_put_fix64_(lst, type_id);
			_enc_put_var32_(lst, 0);
			string val2(_enc_data_(lst), _enc_size_(lst));
			r = database->set(skey, val2);
		}
		else r = ErrorCode::DB_ERROR;
	}
	else r = ErrorCode::ITEM_EXISTED;

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_CREATE for key " << id;
	}
}

void HashService::drop(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(hid, "", first);
		leveldb::ReadOptions options;
		options.fill_cache = false;
		leveldb::Iterator* it = database->db->NewIterator(options);
		it->Seek(first);
		uint32_t i = 0;
		leveldb::WriteBatch batch;
		while (i < count && it->Valid())
		{
			batch.Delete(it->key());
			it->Next();
			i++;
		}
		batch.Delete(skey);
		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_DROP for key " << id;
	}
}

void HashService::size(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	HASH_GET_INFO;

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var32_(rep, count);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_SIZE for key " << id;
	}
}

void HashService::clear(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(hid, "", fk);
		leveldb::ReadOptions options;
		options.fill_cache = false;
		leveldb::Iterator* it = database->db->NewIterator(options);
		it->Seek(fk);
		uint32_t i = 0;
		leveldb::WriteBatch batch;
		while (i < count && it->Valid())
		{
			batch.Delete(it->key());
			it->Next();
			i++;
		}
		count = 0;
		ENCODE_HASH_INFO(info)
		batch.Put(skey, info);
		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_CLEAR for key " << id;
	}
}

void HashService::get(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string key, val;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, key);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(hid, key, k);
		r = database->get(k, &val);
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, val);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_GET for key " << id;
	}
}

void HashService::set(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string key, val;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, key);
	_dec_get_string_(req, val);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		leveldb::WriteBatch batch;
		ENCODE_ITEM_KEY(hid, key, k);
		if (!database->exist(k))
		{
			count++;
			ENCODE_HASH_INFO(info);
			batch.Put(skey, info);
		}

		batch.Put(k, val);
		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_SET for key " << id;
	}
}

void HashService::del(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string key, val;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, key);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		leveldb::WriteBatch batch;
		ENCODE_ITEM_KEY(hid, key, k);

		if (database->exist(k))
		{
			count--;
			ENCODE_HASH_INFO(info);
			batch.Put(skey, info);
			batch.Delete(k);
		}

		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, val);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_GET for key " << id;
	}
}

void HashService::exist(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string key, val;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, key);

	DECODE_CHECK;
	HASH_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(hid, key, k);
		if (!database->exist(k)) r = ErrorCode::IS_NOT_FOUND;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from HASH_EXIST for key " << id;
	}
}

}
