/*
 * ListService.cpp
 *
 *  Created on: Jun 14, 2012
 *      Author: nhsan
 */

#include "ListService.h"

namespace ukibase
{

ListService::ListService()
{
	database = (Database*) Engine::get_instance().get_component(COMP_DBSERVER).get();
	assert(database != NULL);
}

ListService::~ListService()
{

}

bool ListService::process(message_ptr const & message, connection_ptr const & connection)
{
	switch (message->type)
	{
	case MessageType::LIST_CREATE:
		create(message, connection);
		return true;
	case MessageType::LIST_DROP:
		drop(message, connection);
		return true;
	}
	return false;
}

void ListService::create(message_ptr const & message, connection_ptr const & connection)
{

}

void ListService::drop(message_ptr const & message, connection_ptr const & connection)
{

}

} /* namespace ukibase */
