/*
 * Task.h
 *
 *  Created on: Jan 10, 2012
 *      Author: nhsan
 */

#ifndef TASK_H_
#define TASK_H_

#include <string>
#include "Connection.h"
#include "Message.h"
#include "ServerNode.h"

namespace ukicore
{

class Task
{
public:
	Task();
	virtual ~Task();
	/* return true if this task need to be deleted after execute, otherwise return true */
	virtual bool execute(Worker* worker)=0;
};

typedef Task* task_ptr;

/* use when data is available for reading */
class TaskRead: public Task
{
public:
	Connection* connection;
	TaskRead(Connection* connection);
	bool execute(Worker* worker);
};

} /* namespace core */
#endif /* TASK_H_ */
