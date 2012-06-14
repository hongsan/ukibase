/*
 * TaskConnect.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include "TaskConnect.h"
#include "Engine.h"
#include <glog/logging.h>
#include "commons/MessageUtil.h"
#include "commons/Constant.h"
#include "NodeConf.h"
#include "manager.pb.h"

using namespace ukicore;

namespace ukibase
{

TaskConnect::TaskConnect(string host, int port)
{
	this->host = host;
	this->port = port;
}

bool TaskConnect::execute(Worker* worker)
{
	Engine& engine = Engine::get_instance();
	NodeConf* config = (NodeConf*) engine.get_component(COMP_SERVERCONF).get();

	connection_ptr conn = engine.open_connection(host, port);
	if (conn == NULL)
	{
		DLOG(ERROR) << "Can not connect to manager server";
		engine.stop();
		return true;
	}
	config->man_connection = conn;

	/* send hello message to manager server */
	uint64_t msg_id = engine.next_message_id();
	HelloRequest request;
	request.set_id(config->server_id);
	message_ptr msg = Message::pb_encode(request, MessageType::HELLO_REQ, msg_id, 0);
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));
	engine.save_context(context.get());
	conn->send_message(msg);
	context->wait();
	engine.release_context(context.get());

	/* get reply from manager server */
	if (context->done)
	{
		HelloReply reply;
		Message::pb_decode(reply, context->get_reply());

		if (reply.code() == ErrorCode::OK)
		{
			for (int i = 0; i < reply.nodes_size(); i++)
			{
				HelloReply::Nodes n = reply.nodes(i);
				servernode_ptr node = boost::make_shared<ServerNode>(n.id(), n.name(), n.type(), n.host(), n.port());
				config->nodes[n.id()] = node;
				config->ring->add_node(node);

				if (n.id() == config->server_id) config->me = node;
				if (n.online())
				{
					//connect to server
					if (!server_connect(node, config->server_id))
					{
						DLOG(ERROR) << "Can not connect to server:" << node->get_id();
						return true;
					}
				}
			}

			if (config->me == NULL)
			{
				DLOG(ERROR) << "Server node list must contain this node";
				return true;
			}
			engine.listen_connection(config->me->get_ip(), config->me->get_port());
			server_connect(config->me, config->server_id);

			/* send node state to Manager server */
			NodeState state;
			state.set_id(config->server_id);
			state.set_state(NodeState::READY);
			message_ptr msg = Message::pb_encode(state, MessageType::NODE_STATE, 0, 0);
			conn->send_message(msg);
			return true;
		}
	}

	DLOG(ERROR) << "Error in get node list from manager server";
	return true;
}

bool TaskConnect::server_connect(servernode_ptr node, int my_id)
{
	Engine& engine = Engine::get_instance();
	connection_ptr conn = engine.open_connection(node->get_ip(), node->get_port());
	if (conn == NULL)
	{
		DLOG(ERROR) << "Error in server_connect to: " << node->get_ip() << ":" << node->get_port();
		return false;
	}

	node->state = CONNECTED;
	node->connection = conn;
	uint64_t msg_id = engine.next_message_id();
	_enc_declare_(ping, 1024);
	_enc_put_msg_header_(ping, MessageType::PING, msg_id, 0);
	_enc_put_var32_(ping, my_id);
	_enc_update_msg_size_(ping);

	boost::scoped_ptr<ReplyContext> context(new ReplyContext(msg_id));
	engine.save_context(context.get());
	conn->send(_enc_data_(ping), _enc_size_(ping));
	context->wait();
	engine.release_context(context.get());

	if (context->done)
	{
		int code;
		_dec_declare2_(rep, context->get_reply()->get_content_data(), context->get_reply()->get_content_size());
		_dec_get_var32_(rep, code);
		if (_dec_valid_(rep) && code == ErrorCode::OK)
		{
			conn->asyn = true;
			conn->authenticated = true;
			conn->data = node.get();
			conn->type = CT_S2S;
			node->connection = conn;
			node->state = READY;
			return true;
		}
	}
	return false;
}

} /* namespace ukibase */
