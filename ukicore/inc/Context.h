/*
 * MessageTracker.h
 *
 *  Created on: Feb 17, 2012
 *      Author: nhsan
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_
#include "commons/Constant.h"

namespace ukicore
{

class Context
{
private:
	uint64_t id;
	int type;
public:
	Context(uint64_t id, int type)
	{
		this->id = id;
		this->type = type;
	}

	int get_id()
	{
		return id;
	}

	int get_type()
	{
		return type;
	}
};

/* simple context for synchronization */
class ReplyContext: public Context
{
private:
	message_ptr reply;
	boost::mutex mutex;
	boost::condition condition;
public:
	bool done;
	ReplyContext(uint64_t id) :
			Context(id, REPLY_CONTEXT)
	{
		done = false;
	}

	virtual ~ReplyContext()
	{
	}

	/* wait until reply received or timeout */
	void wait(int timeout = 10)
	{
		boost::mutex::scoped_lock lock(mutex);
		if (timeout > 0)
		{
			boost::system_time t = boost::get_system_time() + boost::posix_time::seconds(timeout);
			condition.timed_wait(lock, t);
		}
		else condition.wait(lock);
	}

	/* notify when reply received */
	void notify(message_ptr message)
	{
		done = true;
		this->reply = message;
		condition.notify_all();
	}

	message_ptr get_reply()
	{
		return reply;
	}
};

} /* namespace core */
#endif /* CONTEXT_H_ */
