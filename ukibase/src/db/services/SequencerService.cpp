/*
 * SequencerService.cpp
 *
 *  Created on: Apr 20, 2012
 *      Author: hongsan
 */

#include "SequencerService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
using namespace std;

namespace ukibase
{

SequencerService::SequencerService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

SequencerService::~SequencerService()
{
}

bool SequencerService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::SEQ_CREATE:
		create(message, connection);
		return true;
	case MessageType::SEQ_DESTROY:
		destroy(message, connection);
		return true;
	case MessageType::SEQ_INC:
		inc(message, connection);
		return true;
	case MessageType::SEQ_DEC:
		dec(message, connection);
		return true;
	}
	return false;
}

void SequencerService::create(message_ptr const & message, connection_ptr const & connection)
{
	uint64_t initial;
	uint32_t shard;
	string key;

	string s(message->get_content_data(), message->get_content_size());
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
	_dec_get_string_(req, key);
	_dec_get_fix64_(req, initial);

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
	_enc_put_var32_(key, KeyType::SEQ);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = ErrorCode::OK;
	if (!database->exist(skey))
	{
		_enc_declare_(val2, 32);
		_enc_put_fix64_(val2, initial);
		string val2(_enc_data_(val2), _enc_size_(val2));
		r = database->set(skey, val2);
	}
	else r = ErrorCode::ITEM_EXISTED;


	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_CREATE for key " << key;
	}
}

void SequencerService::destroy(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;

	string s(message->get_content_data(), message->get_content_size());
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
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
	_enc_put_var32_(key, KeyType::SEQ);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = database->del(skey);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_DESTROY for key " << key;
	}
}

void SequencerService::inc(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;

	string s(message->get_content_data(), message->get_content_size());
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
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
	_enc_put_var32_(key, KeyType::SEQ);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	string val;
	int r = database->get(skey, &val);

	uint64_t seq_val;
	if (r == ErrorCode::OK)
	{
		_dec_declare2_(val1, val.c_str(), val.size());
		_dec_get_fix64_(val1, seq_val);
		seq_val++;

		_enc_declare_(val2, 32);
		_enc_put_fix64_(val2, seq_val);
		string val2(_enc_data_(val2), _enc_size_(val2));
		r = database->set(skey, val2);
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_fix64_(rep, seq_val);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_INC for key " << key;
	}
}

void SequencerService::dec(message_ptr const & message, connection_ptr const & connection)
{
	uint32_t shard;
	string key;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_fix32_(req, shard);
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
	_enc_put_var32_(key, KeyType::SEQ);
	_enc_put_buffer_(key, key, key.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	string val;
	int r = database->get(skey, &val);

	uint64_t seq_val;
	if (r == ErrorCode::OK)
	{
		_dec_declare2_(val1, val.c_str(), val.size());
		_dec_get_fix64_(val1, seq_val);
		seq_val--;

		_enc_declare_(val2, 32);
		_enc_put_fix64_(val2, seq_val);
		string val2(_enc_data_(val2), _enc_size_(val2));
		r = database->set(skey, val2);
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_fix64_(rep, seq_val);
	}
	_enc_update_msg_size_(rep);
	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_DEC for key " << key;
	}
}

} /* namespace db */
