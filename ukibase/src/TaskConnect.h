/*
 * TaskConnect.h
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#ifndef TASKCONNECT_H_
#define TASKCONNECT_H_

#include "Task.h"
#include <string>

using namespace std;
using namespace ukicore;
namespace ukibase
{

class TaskConnect : public Task
{
public:
	string host;
	int port;
	TaskConnect(string host, int port);
	bool execute(Worker* worker);
	bool server_connect(servernode_ptr node, int my_id);
};

} /* namespace ukibase */
#endif /* TASKCONNECT_H_ */
