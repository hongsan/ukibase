/*
 * Database.h
 *
 *  Created on: Apr 13, 2012
 *      Author: hongsan
 */

#ifndef DATABASE_H_
#define DATABASE_H_
#include "Engine.h"
#include "leveldb/db.h"
#include "HashRing.h"
#include <string>
#include <google/dense_hash_map>
#include <list>
using namespace std;
using namespace ukicore;

namespace ukibase
{

typedef google::dense_hash_map<string, list<message_ptr> > lock_map;

class Database: public Component
{
private:
	lock_map locks;
	boost::mutex lock_mutex;
public:
	leveldb::Options options;
	leveldb::DB* db;
	servernode_ptr me;
	hashring_ptr ring;

	Database();
	virtual ~Database();

	void init();

	int set(leveldb::Slice& key, string& val);
	int get(leveldb::Slice& key, string* val);
	int del(leveldb::Slice& key);
	bool exist(leveldb::Slice& key);

	int list_create(string& key);
	int list_destroy(string& key);
	int list_pushback(string& key, string& val);
	int list_pushfront(string& key, string& val);
	int list_size(string& key, uint32_t& size);
	int list_index(string& key, uint32_t index, string& val);
	int list_remove(string& key, uint32_t index);
	int list_range(string& key, uint32_t index, uint32_t& count, char* data, uint32_t& data_size);
	int list_clear(string& key);
};

#define DECODE_CHECK\
	if (!_dec_valid_(req))\
	{\
		_enc_declare_(rep, 32);\
		_enc_put_msg_header_(rep, MessageType::REPLY, message->id, 0);\
		_enc_put_var32_(rep, ErrorCode::BAD_FORMAT);\
		_enc_update_msg_size_(rep);\
		connection->send(_enc_data_(rep), _enc_size_(rep));\
		return;\
	}

} /* namespace db */
#endif /* DATABASE_H_ */
