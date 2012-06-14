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
	uint64_t next_type_id();

	/* sequence */
	int seq_create(string id, uint64_t init, bool asc = true);
	int seq_drop(string id);
	int seq_next(string id, uint64_t& val);

	/* collection */
	int col_create(string id, uint64_t& cid);
	int col_drop(string id);
	int col_id(string id, uint64_t& cid);
	int col_get(uint64_t cid, string key, string& val);
	int col_put(uint64_t cid, string key, string val);
	int col_del(uint64_t cid, string key);
	int col_exist(uint64_t cid, string key);

	/* list */
	int list_create(string id);
	int list_drop(string id);
	int list_size(string id, uint32_t& size);
	int list_clear(string id);
	int list_push_back(string id, string val);
	int list_push_front(string id, string val);
	int list_index(string id, uint32_t index, string& val);
	int list_remove(string id, uint32_t index);
	int list_range(string id, uint32_t index, uint32_t count, vector<string>& vals);
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

