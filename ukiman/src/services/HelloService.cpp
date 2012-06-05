/*
 * HelloService.cpp
 *
 *  Created on: Jun 5, 2012
 *      Author: nhsan
 */

#include "HelloService.h"
#include <glog/logging.h>
#include "Engine.h"
#include "manager.pb.h"
#include "../Manager.h"

namespace ukiman
{

HelloService::HelloService()
{
}

HelloService::~HelloService()
{
}

bool HelloService::process(message_ptr const & message, connection_ptr const & connection)
{
	static Engine& engine = Engine::get_instance();
	switch (message->type)
	{
	case MessageType::HELLO_REQ:
	{
		DLOG(INFO) << "Received Hello request";
		HelloRequest request;
		if (!Message::pb_decode(request, message))
		{
			DLOG(ERROR) << "Bad hello request";
			return true;
		}

		HelloReply reply;
		reply.set_code(ErrorCode::OK);
		Manager* manager = (Manager*) engine.get_component(COMP_MANAGER).get();

		for (int i = 0; i < manager->nodes.size(); i++)
		{
			HelloReply_Nodes* node = reply.add_nodes();
			node->set_id(manager->nodes[i]->get_id());
			node->set_type(manager->nodes[i]->get_type());
			node->set_host(manager->nodes[i]->get_ip());
			node->set_port(manager->nodes[i]->get_port());
			node->set_name(manager->nodes[i]->get_name());

			if (manager->nodes[i]->state == ONLINE || manager->nodes[i]->state == READY || manager->nodes[i]->state == CONNECTED)
			{
				node->set_online(true);
			}
			else node->set_online(false);

			if (manager->nodes[i]->get_id() == request.id())
			{
				connection->authenticated = true;
				connection->data = manager->nodes[i].get();
				connection->asyn = true;
				connection->type = CT_S2S;
				manager->nodes[i]->connection = connection;
				manager->nodes[i]->state = CONNECTED;
			}

		}

		message_ptr msg = Message::pb_encode(reply, MessageType::REPLY, message->id, 0);
		connection->send_message(msg);
		return true;
	}

	case MessageType::NODE_STATE:
		Manager* manager = (Manager*) engine.get_component(COMP_MANAGER).get();
		NodeState state;
		if (Message::pb_decode(state, message) && (manager != NULL))
		{
			for (int i = 0; i < manager->nodes.size(); i++)
			{
				if (manager->nodes[i]->get_id() == state.id())
				{
					if (state.state() == NodeState::ONLINE) manager->nodes[i]->state = ONLINE;
					DLOG(INFO) << "Node " << state.id() << " is online";
				}
			}
		}
		return true;
	}
	return false;
}

} /* namespace ukibase */

