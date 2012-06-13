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
#include "Collection.h"

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
	int inc(uint32_t type, string key, uint64_t& val);

	/* collection API */
	collection_ptr get_collection(string id);
	int create_collection(string id);
	int drop_collection();
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
