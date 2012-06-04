/*
 * Connection.cpp
 *
 *  Created on: Dec 13, 2011
 *      Author: NGUYEN Hong San
 */

#include "Connection.h"
#include <glog/logging.h>
#include "Engine.h"
#include "Task.h"
#include "Message.h"
#include <boost/make_shared.hpp>
#define MAXTRIES 100

namespace ukicore
{

Connection::Connection(int fd)
{
	this->fd = fd;
	asyn = true;
	received = 0;
	processing = false;
	some_event_come = false;
	authenticated = false;
	task_read = new TaskRead(this);
}

Connection::~Connection()
{
	delete task_read;
}

int Connection::get_socket()
{
	return fd;
}

int Connection::send(char* data, size_t size)
{
	boost::mutex::scoped_lock slock(send_mutex);
	int sent = 0;
	int tries = 0;
	while (sent < size)
	{
		int n = write(fd, data + sent, (size - sent));
		if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				tries++;
				if (tries > MAXTRIES)
				{
					DLOG(ERROR) << "Network may has some problems";
					return -1;
				}
				usleep(100);
			}
			else return -1;
		}
		else
		{
			sent += n;
			tries = 0;
		}
	}
	return size;
}

int Connection::send_message(message_ptr const & message)
{
	boost::mutex::scoped_lock slock(send_mutex);
	int n = write(fd, message->get_data(), message->size);
	return n;
}

void Connection::notify_read()
{
	static TaskQueue& tasks = Engine::get_instance().get_tasks();
	boost::mutex::scoped_lock lock(check_mutex);
	if (processing) watch_dog++;
	else
	{
		tasks.enqueue(task_read);
		processing = true;
	}
}

bool Connection::read_message(Worker* worker)
{
	static Engine& server = Engine::get_instance();
	static TaskQueue& tasks = server.get_tasks();

	while (1) //loop until no more data in socket
	{
		watch_dog = 0;
		if (received < PACKET_HEADER_SIZE)
		{
			/* read header data */
			int count = read(fd, header, PACKET_HEADER_SIZE - received);
			if (count == 0)
			{
				DLOG(INFO) << "Close connection:" << fd;
				return false;
			}
			else if (count == -1)
			{
				if (errno != EAGAIN)
				{
					DLOG(ERROR) << "Something wrong with connection:" << fd;
					return false;
				}
				else
				{
					boost::mutex::scoped_lock lock(check_mutex);
					if (watch_dog == 0)
					{
						processing = false;
						break;
					}
					else continue;
				}
			}

			received += count;
			if (received == PACKET_HEADER_SIZE)
			{
				if (header[0] != PACKET_MAGIC || header[1] != PACKET_MAGIC)
				{
					DLOG(ERROR) << "Magic bytes are not matched";
					//TODO: handle error
				}

				int size, type;
				size = (header[2] << 8) + header[3]; // 2 bytes for size
				type = (header[4] << 16) + (header[5] << 8) + header[6]; //3 bytes for type

				reading_message = boost::make_shared<BufferMessage>(size, type);
				memcpy(reading_message->get_data(), header, PACKET_HEADER_SIZE);
			}
		}
		else if (received < reading_message->size) //size here mean packet size not content size
		{
			/* read message data */
			int count = read(fd, reading_message->get_data() + received, reading_message->size - received);
			if (count == 0)
			{
				//DLOG(INFO) << "Close connection:" << ev->fd;
				reading_message.reset();
				return false;
			}
			else if (count == -1)
			{
				if (errno != EAGAIN)
				{
					DLOG(ERROR) << "Something wrong with connection:" << fd;
					reading_message.reset();
					return false;
				}
				else
				{
					boost::mutex::scoped_lock lock(check_mutex);
					if (watch_dog == 0)
					{
						processing = false;
						break;
					}
					else continue;
				}
			}

			received += count;
		}
		else if (received == reading_message->size)
		{
			/* decode message_id and streaming type*/
			bool has_id, has_okey;
			has_okey = ((header[7] & 0x01) == 1);
			has_id = ((header[7] & 0x02) == 2);

			unsigned char* tmp_data = (unsigned char*) (reading_message->get_data() + 8);


			if (has_okey)
			{
				uint32_t okey = tmp_data[0];
				okey <<= 8;
				okey += tmp_data[1];
				okey <<= 8;
				okey += tmp_data[2];
				okey <<= 8;
				okey += tmp_data[3];
				tmp_data += 4;
				reading_message->okey = okey;
			}
			else reading_message->okey = 0;

			if (has_id)
			{
				uint64_t message_id = tmp_data[0];
				message_id <<= 8;
				message_id += tmp_data[1];
				message_id <<= 8;
				message_id += tmp_data[2];
				message_id <<= 8;
				message_id += tmp_data[3];
				message_id <<= 8;
				message_id += tmp_data[4];
				message_id <<= 8;
				message_id += tmp_data[5];
				message_id <<= 8;
				message_id += tmp_data[6];
				message_id <<= 8;
				message_id += tmp_data[7];
				reading_message->id = message_id;
			}
			else reading_message->id = 0;

			if (!asyn)
			{
				server.process_message(reading_message, me.lock()); //process immediately
			}
			else if (reading_message->okey > 0) //ordering request
			{
				bool working;
				message_ptr msg = reading_message;
				server.mqueue_push(msg, worker, working);

				/* reset reading message and push to queue*/
				received = 0;
				reading_message.reset();
				tasks.enqueue(task_read);

				if (!working)
				{
					while (msg != NULL)
					{
						//DLOG(INFO) << "Process message, okey = " << msg->okey << " ,id = " << msg->id;
						server.process_message(msg, me.lock());
						msg = server.mqueue_pop(worker->okey);
					}
				}
				return true;
			}
			else
			{
				message_ptr msg = reading_message;

				/* reset reading message and push to queue*/
				received = 0;
				reading_message.reset();
				tasks.enqueue(task_read);

				//DLOG(INFO) << "Process message, okey = " << msg->okey << " ,id = " << msg->id;
				server.process_message(msg, me.lock());
				return true;
			}
			received = 0; //ready for new packet
			reading_message.reset();
		}
	}

	return true;
}

} /* namespace core */
