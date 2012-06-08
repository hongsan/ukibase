/*
 * Database.h
 *
 *  Created on: Apr 13, 2012
 *      Author: hongsan
 */

#ifndef DATABASE_H_
#define DATABASE_H_
#include "HashRing.h"
#include <string>
#include "Component.h"
using namespace std;
using namespace ukicore;
#include "commons/MurmurHash3.h"
#include "../NodeConf.h"

namespace dbclient
{

class Database: public Component
{
private:
	NodeConf* config;
public:
	Database();
	virtual ~Database();

	void init();

	/* general object API */
	int set(uint32_t type, string key, string val);
	int get(uint32_t type, string key, string* val);
	int del(uint32_t type, string key);
	int exist(uint32_t type, string key);

	/* sequencer API */
	int seq_create(string key, uint64_t initial);
	int seq_destroy(string key);
	int seq_inc(string key, uint64_t& val);
	int seq_dec(string key, uint64_t& val);

	/* list API */
	int list_create(string key);
	int list_destroy(string key);
	int list_push_back(string key, string val);
	int list_push_front(string key, string val);
	int list_mpush_back(string key, vector<string>& vals);
	int list_mpush_front(string key, vector<string>& vals);
	int list_size(string key, uint32_t& size);
	int list_range(string key, int index, int count, vector<string>& vals);
	int list_remove(string key, int index);
	int list_index(string key, int index, string& val);
	int list_clear(string key);

	/* hash API */
	int hash_create(string key);
	int hash_destroy(string key);
	int hash_set(string key, string ikey, string ival);
	int hash_get(string key, string ikey, string& ival);
	int hash_del(string key, string ikey);
	int hash_exist(string key, string ikey);
	int hash_size(string key, string ikey);
	int hash_vals(string key, vector<string>& vals);
	int hash_keys(string key, vector<string>& keys);
	int hash_data(string key, map<string, string>& data);
	int hash_mget(string key, vector<string> ikeys, map<string, string>& ivals);
	int hash_mset(string key, map<string, string>& data);

	/* filedb */
	int create(int size, string& key);
	int write(string key, int offset, int size, const char* data);
	int read(string key, int offset, int size, char* data);
	int size(string key, int& size);
};

#define DO_REQUEST_REPLY\
	servernode_ptr node = config->ring->get_node(shard);\
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));\
	engine.save_context(context.get());\
	node->connection->send(_enc_data_(req), _enc_size_(req));\
	context->wait();\
	engine.release_context(context.get());\
	if (!context->done) return ErrorCode::TIMEOUT;\
	int code;\
	_dec_declare2_(rep, context->get_reply()->get_content_data(), context->get_reply()->get_content_size());\
	_dec_get_var32_(rep, code);\

} /* namespace db */
#endif /* DATABASE_H_ */
