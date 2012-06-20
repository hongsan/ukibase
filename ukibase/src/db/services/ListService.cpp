/*
 * ListService.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: nhsan
 */

#include "ListService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
#include "leveldb/write_batch.h"
using namespace std;

#define HALF_INDEX	(1<<31)

#define ENCODE_LIST_KEY(skey)\
_enc_declare_(key, id.size() + 32);\
_enc_put_var64_(key, METADATA_TYPE);\
_enc_put_var64_(key, DataType::LIST);\
_enc_put_buffer_(key, id, id.size());\
leveldb::Slice skey(_enc_data_ (key), _enc_size_ (key));

#define LIST_GET_INFO \
ENCODE_LIST_KEY(skey);\
uint64_t lid;\
uint32_t count, first, last;\
string sval;\
int r = database->get(skey, &sval);\
if (r == ErrorCode::OK)\
{\
	_dec_declare1_(info, sval);\
	_dec_get_fix64_(info, lid);\
	_dec_get_var32_(info, count);\
	_dec_get_var32_(info, first);\
	_dec_get_var32_(info, last);\
	if (!_dec_valid_(info)) r = ErrorCode::DB_ERROR;\
}

#define ENCODE_LIST_INFO(info)\
_enc_declare_(info, 64);\
_enc_put_fix64_(info, lid);\
_enc_put_var32_(info, count);\
_enc_put_var32_(info, first);\
_enc_put_var32_(info, last);\
leveldb::Slice info(_enc_data_(info), _enc_size_(info));

#define ENCODE_ITEM_KEY(lid, index, key)\
_enc_declare_(k, 64);\
_enc_put_var64_(k, lid);\
_enc_put_fix32_(k, index);\
leveldb::Slice key(_enc_data_(k), _enc_size_(k));

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
	case MessageType::LIST_DROP:
		drop(message, connection);
		return true;
	case MessageType::LIST_SIZE:
		size(message, connection);
		return true;
	case MessageType::LIST_PUSH_BACK:
		push_back(message, connection);
		return true;
	case MessageType::LIST_PUSH_FRONT:
		push_front(message, connection);
		return true;
	case MessageType::LIST_REMOVE:
		remove(message, connection);
		return true;
	case MessageType::LIST_CLEAR:
		clear(message, connection);
		return true;
	case MessageType::LIST_INDEX:
		index(message, connection);
		return true;
	}

	return false;
}

void ListService::index(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	uint32_t index;
	string item;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_var32_(req, index);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(lid, first, fk);
		leveldb::ReadOptions options;
		options.fill_cache = false;
		leveldb::Iterator* it = database->db->NewIterator(options);
		it->Seek(fk);
		uint32_t i = 0;
		r = ErrorCode::IS_NOT_FOUND;
		while (i < count && it->Valid())
		{
			if (i == index)
			{
				item = it->value().ToString();
				r = ErrorCode::OK;
				break;
			}
			it->Next();
			i++;
		}
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_string_(rep, item);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from LIST_INDEX for key " << id;
	}
}

void ListService::clear(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(lid, first, fk);
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
		first = HALF_INDEX;
		last = HALF_INDEX;
		ENCODE_LIST_INFO(info)
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
		DLOG(ERROR) << "Send error from LIST_CLEAR for key " << id;
	}
}

void ListService::remove(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	uint32_t index;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_var32_(req, index);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(lid, first, fk);
		leveldb::ReadOptions options;
		//options.fill_cache = false;
		leveldb::Iterator* it = database->db->NewIterator(options);
		it->Seek(fk);
		uint32_t i = 0;
		while (i < count && it->Valid())
		{
			if (i == index)
			{
				leveldb::WriteBatch batch;
				count--;
				ENCODE_LIST_INFO(info);
				batch.Put(skey, info);
				batch.Delete(it->key());
				leveldb::Status status = database->db->Write(leveldb::WriteOptions(), &batch);
				if (!status.ok()) r = ErrorCode::DB_ERROR;
				break;
			}
			it->Next();
			i++;
		}
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from LIST_REMOVE for key " << id;
	}
}

void ListService::push_back(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string item;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, item);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		last++;
		//TODO: rang check
		count++;
		ENCODE_LIST_INFO(info);
		ENCODE_ITEM_KEY(lid, last, key)

		leveldb::WriteBatch batch;
		batch.Put(skey, info);
		batch.Put(key, item);
		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from LIST_PUSHBACK for key " << id;
	}
}

void ListService::push_front(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	string item;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_string_(req, item);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		first--;
		//TODO: rang check
		count++;
		ENCODE_LIST_INFO(info);
		ENCODE_ITEM_KEY(lid, first, key)

		leveldb::WriteBatch batch;
		batch.Put(skey, info);
		batch.Put(key, item);
		leveldb::Status s = database->db->Write(leveldb::WriteOptions(), &batch);
		if (!s.ok()) r = ErrorCode::DB_ERROR;
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from LIST_PUSHFRONT for key " << id;
	}
}

void ListService::size(message_ptr const & message, connection_ptr const & connection)
{
	string id;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	LIST_GET_INFO;

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
		DLOG(ERROR) << "Send error from LIST_DROP for key " << id;
	}
}

void ListService::create(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	ENCODE_LIST_KEY(skey);

	int r = ErrorCode::OK;
	uint64_t type_id = 0;
	if (!database->exist(skey))
	{
		type_id = database->next_type_id();

		//DLOG(INFO) << "typeid=" << type_id;
		if (type_id > 0)
		{
			_enc_declare_(lst, 64);
			_enc_put_fix64_(lst, type_id);
			_enc_put_var32_(lst, 0);
			_enc_put_var32_(lst, HALF_INDEX);
			_enc_put_var32_(lst, HALF_INDEX);
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
		DLOG(ERROR) << "Send error from LIST_CREATE for key " << id;
	}
}

void ListService::drop(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;
	LIST_GET_INFO;

	if (r == ErrorCode::OK)
	{
		ENCODE_ITEM_KEY(lid, first, fk);
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
		DLOG(ERROR) << "Send error from LIST_DROP for key " << id;
	}
}

} /* namespace ukibase */
