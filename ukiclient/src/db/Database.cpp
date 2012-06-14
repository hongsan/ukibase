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

uint64_t Database::next_type_id()
{
	Engine& engine = Engine::get_instance();
	uint32_t shard;
	string id = SEQ_DATATYPE;
	MurmurHash3_x86_32(id.c_str(), id.size(), DataType::SEQUENCE, &shard);

	int size = id.size() + 32;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(req, size);
	_enc_put_msg_header_(req, MessageType::SEQ_TYPE, msg_id, shard);
	_enc_put_string_(req, id);
	_enc_update_msg_size_(req);

	servernode_ptr node = config->ring->get_node(shard);
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));
	engine.save_context(context.get());
	node->connection->send(_enc_data_(req), _enc_size_(req));
	context->wait();
	engine.release_context(context.get());
	if (!context->done) return 0;

	int code;
	_dec_declare2_(rep, context->get_reply()->get_content_data(), context->get_reply()->get_content_size());
	_dec_get_var32_(rep, code);
	uint64_t val;
	if (code == ErrorCode::OK)
	{
		_dec_get_var64_(rep, val);
	}
	if (!_dec_valid_(rep)) return 0;
	if (code != ErrorCode::OK) return 0;
	return val;
}

} /* namespace db */
