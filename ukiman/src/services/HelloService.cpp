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
	if (message->type != MessageType::HELLO_REQ) return false;
	DLOG(INFO)<<"Received Hello request";
	HelloRequest request;
	if (!Message::pb_decode(request,message))
	{
		DLOG(ERROR)<<"Bad hello request";
		return true;
	}
	return true;
}

} /* namespace ukibase */
