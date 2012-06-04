/*
 * Worker.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#include "Worker.h"
#include <glog/logging.h>
#include <boost/lexical_cast.hpp>
#include <stdlib.h>
#include "commons/SocketUtil.h"
#include "commons/Constant.h"
#include "Engine.h"

namespace ukicore
{

Worker::Worker(int id)
{
	this->id = id;
}

Worker::~Worker()
{
}

void Worker::run()
{
	DLOG(INFO) << "Worker " << id << " has started";
	Engine& server = Engine::get_instance();
	TaskQueue& tasks = server.get_tasks();

	while (server.running)
	{
		task_ptr t = tasks.dequeue();
		if (t == NULL) continue;
		if (t->execute(this)) delete t;
	}
	DLOG(INFO) << "Worker " << id << " is stopped";
}

} /* namespace core */
