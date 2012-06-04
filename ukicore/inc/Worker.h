/*
 * Worker.h
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#ifndef WORKER_H_
#define WORKER_H_
#include "commons/ThreadBase.h"
#include "Message.h"
#include <list>
using namespace std;

namespace ukicore
{

class Worker: public ThreadBase
{
private:
	int id;
public:
	list<message_ptr> messages;
	uint32_t okey;

	Worker(int id);
	virtual ~Worker();
	void run();
};

typedef boost::shared_ptr<Worker> worker_ptr;
} /* namespace core */
#endif /* WORKER_H_ */
