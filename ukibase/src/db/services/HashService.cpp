/*
 * HashService.cpp
 *
 *  Created on: Jun 21, 2012
 *      Author: nhsan
 */

#include "HashService.h"
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include <string>
#include <glog/logging.h>
#include "leveldb/write_batch.h"
using namespace std;

namespace ukibase
{

HashService::HashService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

HashService::~HashService()
{

}

bool HashService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::HASH_CREATE:
		create(message, connection);
		return true;
	case MessageType::HASH_DROP:
		drop(message, connection);
		return true;
	}
	return false;
}

void HashService::create(message_ptr const & message, connection_ptr const & connection)
{

}

void HashService::drop(message_ptr const & message, connection_ptr const & connection)
{

}

}
