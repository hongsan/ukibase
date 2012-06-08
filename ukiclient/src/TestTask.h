/*
 * TestTask.h
 *
 *  Created on: Jun 8, 2012
 *      Author: nhsan
 */

#ifndef TESTTASK_H_
#define TESTTASK_H_

#include "Task.h"
#include <string>
using namespace std;
using namespace ukicore;

namespace dbclient
{

class TestTask: public Task
{
private:

public:
	TestTask();
	virtual ~TestTask();
	bool execute(Worker* worker);
};

} /* namespace ukibase */
#endif /* TESTTASK_H_ */
