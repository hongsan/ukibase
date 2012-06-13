/*
 * Database.cpp
 *
 *  Created on: Apr 13, 2012
 *      Author: hongsan
 */

#include "Database.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"

namespace dbclient
{

Database::Database() :
		Component(COMP_DBCLIENT)
{
}

Database::~Database()
{

}

void Database::init()
{
	config = (NodeConf*) Engine::get_instance().get_component(COMP_SERVERCONF).get();
	assert(config != NULL);
}

//int Database::seq_next(string id, uint64_t& val)
//{
//		Engine& engine = Engine::get_instance();
//		uint32_t shard;
//		MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);
//
//		int size = id.size() + 32;
//		uint64_t msg_id = engine.next_message_id();
//		_enc_declare_(req, size);
//		_enc_put_msg_header_(req, MessageType::SEQ_NEXT, msg_id, shard);
//		_enc_put_string_(req, id);
//		_enc_update_msg_size_(req);
//
//		DO_REQUEST_REPLY;
//		if (code==ErrorCode::OK)
//		{
//			_dec_get_var64_(rep,val);
//		}
//		if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
//		return code;
//}

//int Database::set(uint64_t type, string key, string val)
//{
//	Engine& engine = Engine::get_instance();
//	uint32_t shard;
//	MurmurHash3_x86_32(key.c_str(), key.size(), 0, &shard);
//
//	int size = key.size() + val.size() + 32;
//	uint64_t msg_id = engine.next_message_id();
//	_enc_declare_(req, size);
//	_enc_put_msg_header_(req, MessageType::OBJ_SET, msg_id, 0);
//	_enc_put_var64_(req, type);
//	_enc_put_string_(req, key);
//	_enc_put_string_(req, val);
//	_enc_update_msg_size_(req);
//
//	DO_REQUEST_REPLY;
//
//	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
//	return code;
//}
//
//int Database::get(uint64_t type, string key, string& val)
//{
//	Engine& engine = Engine::get_instance();
//	uint32_t shard;
//	MurmurHash3_x86_32(key.c_str(), key.size(), 0, &shard);
//
//	int size = key.size() + 32;
//	uint64_t msg_id = engine.next_message_id();
//	_enc_declare_(req, size);
//	_enc_put_msg_header_(req, MessageType::OBJ_GET, msg_id, 0);
//	_enc_put_var64_(req, type);
//	_enc_put_string_(req, key);
//	_enc_update_msg_size_(req);
//
//	DO_REQUEST_REPLY;
//
//	if (code == ErrorCode::OK)
//	{
//		_dec_get_string_(rep, val);
//	}
//	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
//	return code;
//}
//
//int Database::del(uint64_t type, string key)
//{
//	Engine& engine = Engine::get_instance();
//	uint32_t shard;
//	MurmurHash3_x86_32(key.c_str(), key.size(), 0, &shard);
//
//	int size = key.size() + 32;
//	uint64_t msg_id = engine.next_message_id();
//	_enc_declare_(req, size);
//	_enc_put_msg_header_(req, MessageType::OBJ_DEL, msg_id, 0);
//	_enc_put_var64_(req, type);
//	_enc_put_string_(req, key);
//	_enc_update_msg_size_(req);
//
//	DO_REQUEST_REPLY;
//
//	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
//	return code;
//}
//
//int Database::inc(uint64_t type, string key, uint64_t& val)
//{
//	Engine& engine = Engine::get_instance();
//	uint32_t shard;
//	MurmurHash3_x86_32(key.c_str(), key.size(), 0, &shard);
//
//	int size = key.size() + 32;
//	uint64_t msg_id = engine.next_message_id();
//	_enc_declare_(req, size);
//	_enc_put_msg_header_(req, MessageType::OBJ_INC, msg_id, shard);
//	_enc_put_var64_(req, type);
//	_enc_put_string_(req, key);
//	_enc_update_msg_size_(req);
//
//	DO_REQUEST_REPLY;
//	if (code==ErrorCode::OK)
//	{
//		_dec_get_var64_(rep,val);
//	}
//	if (!_dec_valid_(rep)) return ErrorCode::IO_ERROR;
//	return code;
//}

} /* namespace db */
