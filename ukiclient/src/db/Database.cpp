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

Database::Database(): Component(COMP_DBCLIENT)
{
	 config = (NodeConf*)Engine::get_instance().get_component(COMP_SERVERCONF).get();
}

Database::~Database()
{
}

void Database::init()
{

}

} /* namespace db */
