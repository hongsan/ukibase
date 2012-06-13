/*
 * db_sequence.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: nhsan
 */

#include "Database.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"

namespace dbclient
{
int Database::seq_create(string id, uint64_t init, bool asc)
{
	return 0;
}

int Database::seq_drop(string id, uint64_t& val)
{
	return 0;
}

int Database::seq_next(string id, uint64_t& val)
{
	return 0;
}

}

