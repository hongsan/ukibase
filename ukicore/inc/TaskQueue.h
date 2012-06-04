/*
 * MessageQueue.h
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "Task.h"
#include <google/dense_hash_map>
#include <list>
using namespace std;

namespace ukicore
{

typedef google::dense_hash_map<string, list<message_ptr> > lock_map;

class TaskQueue
{
private:
	task_ptr* data;
	task_ptr* first;
	task_ptr* last;
	task_ptr* next;
	task_ptr* end_of_data;

	boost::mutex read_mutex;
	boost::mutex write_mutex;
	boost::condition empty_cond;
	boost::condition full_cond;
	bool end;

	lock_map locks;
	boost::mutex lock_mutex;
public:
	TaskQueue();
	void init(int size);
	virtual ~TaskQueue();
	void enqueue(task_ptr task);
	task_ptr dequeue();
	bool empty();
	void start();
	void stop();

	void put_message(message_ptr  message);
	message_ptr get_message(string key);
};

} /* namespace chat */
#endif /* MESSAGEQUEUE_H_ */
