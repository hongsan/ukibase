/*
 * Session.h
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#ifndef SESSION_H_
#define SESSION_H_
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <queue>
#include "Message.h"

namespace ukicore
{

class Message;
class TaskRead;
class Worker;

enum ConnectionType
{
        CT_C2S, CT_S2S
};

class Connection
{
private:
	boost::mutex send_mutex; /* manage sending concurrence */
	boost::mutex check_mutex;
	bool some_event_come;
	bool processing;
	unsigned char header[PACKET_HEADER_SIZE];
	int received; //received data
	message_ptr reading_message;
	int fd;
	int watch_dog;
	TaskRead* task_read;

public:
	bool asyn;
	void* data;
	bool authenticated;
	boost::weak_ptr<Connection> me;
	ConnectionType type;

	Connection(int fd);
	virtual ~Connection();
	int get_socket();
	int send(char* data, size_t size);
	int send_message(message_ptr const & message);

	bool read_message(Worker* worker);
	void notify_read();
};

typedef boost::shared_ptr<Connection> connection_ptr;

} /* namespace core */
#endif /* SESSION_H_ */
