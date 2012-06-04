/*
 * MessageQueue.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#include "TaskQueue.h"
#include <glog/logging.h>

namespace ukicore
{

TaskQueue::TaskQueue()
{
	end = true;
}

void TaskQueue::init(int size)
{
	data = (task_ptr*) malloc(size * sizeof(task_ptr));
	first = data;
	last = first;
	next = last + 1;
	end_of_data = data + size;
}

TaskQueue::~TaskQueue()
{
	if (data) free(data);
}

void TaskQueue::enqueue(task_ptr task)
{
	boost::mutex::scoped_lock wlock(write_mutex);
	while (next == first) //full
	{
		full_cond.wait(wlock);
		if (end) return;
	}

	*last = task;
	last = next;
	next++;
	if (next == end_of_data) next = data;
	empty_cond.notify_one();
}

void TaskQueue::start()
{
	end = false;
}

void TaskQueue::stop()
{
	end = true;
	empty_cond.notify_all();
	full_cond.notify_all();
}

task_ptr TaskQueue::dequeue()
{
	bool full = false;
	task_ptr ret = NULL;

	boost::mutex::scoped_lock rlock(read_mutex);
	while (last == first) //empty
	{
		empty_cond.wait(rlock); //wait until queue is not empty
		if (end) return NULL;
	}

	if (next == first) full = true;
	ret = *first;
	first++;
	if (first == end_of_data) first = data;
	if (full) full_cond.notify_one();

	return ret;
}

bool TaskQueue::empty()
{
	return (last == first);
}


} /* namespace chat */
