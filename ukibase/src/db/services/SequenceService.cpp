/*
 * SequenceService.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: nhsan
 */

#include "SequenceService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
using namespace std;

namespace ukibase
{

SequenceService::SequenceService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

SequenceService::~SequenceService()
{

}

void SequenceService::create(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	uint64_t init;
	unsigned char asc;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	_dec_get_var64_(req, init);
	_dec_get_byte_(req, asc);

	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::SEQUENCE);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = ErrorCode::OK;
	if (!database->exist(skey))
	{
		_enc_declare_(seq, 64);
		_enc_put_byte_(seq, asc)
		_enc_put_fix64_(seq, init);
		string val2(_enc_data_(seq), _enc_size_(seq));
		r = database->set(skey, val2);
	}
	else r = ErrorCode::ITEM_EXISTED;

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_CREATE for key " << id;
	}
}

void SequenceService::drop(message_ptr const & message, connection_ptr const & connection)
{
	string id;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::SEQUENCE);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	int r = database->del(skey);
	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_DROP for key " << id;
	}
}

void SequenceService::next(message_ptr const & message, connection_ptr const & connection)
{
	string id;

	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);
	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::SEQUENCE);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	string val;
	int r = database->get(skey, &val);

	uint64_t seq_val;
	if (r == ErrorCode::OK)
	{
		unsigned char asc;
		_dec_declare2_(val1, val.c_str(), val.size());
		_dec_get_byte_(val1, asc);
		_dec_get_fix64_(val1, seq_val);
		if (asc) seq_val++;
		else seq_val--;

		_enc_declare_(val2, 32);
		_enc_put_byte_(val2, asc);
		_enc_put_fix64_(val2, seq_val);
		string val2(_enc_data_(val2), _enc_size_(val2));
		r = database->set(skey, val2);
	}

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var64_(rep, seq_val);
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_NEXT for key " << id;
	}
}

void SequenceService::type(message_ptr const & message, connection_ptr const & connection)
{
	string id;
	_dec_declare2_(req, message->get_content_data(), message->get_content_size());
	_dec_get_string_(req, id);

	DECODE_CHECK;

	_enc_declare_(key, id.size() + 32);
	_enc_put_var64_(key, METADATA_TYPE);
	_enc_put_var64_(key, DataType::SEQUENCE);
	_enc_put_buffer_(key, id, id.size());
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

	uint64_t ival = 0;
	string sval;
	int r = database->get(skey, &sval);
	if (r == ErrorCode::OK)
	{
		_dec_declare1_(val, sval);
		_dec_get_fix64_(val, ival);
		if (!_dec_valid_(val))
		{
			_enc_declare_(rep, 32);
			_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
			_enc_put_var32_(rep, ErrorCode::DB_ERROR);
			_enc_update_msg_size_(rep);
			return;
		}
	}

	ival++;
	_enc_declare_(val, 32);
	_enc_put_fix64_(val, ival);
	string res(_enc_data_(val), _enc_size_(val));

	r = database->set(skey, res);

	_enc_declare_(rep, 32);
	_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);
	_enc_put_var32_(rep, r);
	if (r == ErrorCode::OK)
	{
		_enc_put_var64_(rep, ival)
	}
	_enc_update_msg_size_(rep);

	if (connection->send(_enc_data_(rep), _enc_size_(rep)) < 0)
	{
		DLOG(ERROR) << "Send error from SEQ_TYPE for key " << id;
	}
}

bool SequenceService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::SEQ_CREATE:
		create(message, connection);
		return true;
	case MessageType::SEQ_DROP:
		drop(message, connection);
		return true;
	case MessageType::SEQ_NEXT:
		next(message, connection);
		return true;
	case MessageType::SEQ_TYPE:
		type(message, connection);
		return true;
	}
	return false;
}

} /* namespace ukibase */
