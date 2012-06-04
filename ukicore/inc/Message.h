/*
 * Message.h
 *
 *  Created on: Jan 10, 2012
 *      Author: nhsan
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <stdint.h>
#include <string.h>

#define PACKET_HEADER_SIZE		8
#define PACKET_MAGIC			0xEE

namespace ukicore
{

class Message;
typedef boost::shared_ptr<Message> message_ptr;

class Message
{
public:
	int size;
	int type;
	uint64_t id;
	uint32_t okey;

	Message()
	{
		id = 0;
		okey = 0;
	}

	virtual ~Message()
	{
	}
	virtual char* get_data()=0;

	char* get_content_data()
	{
		char* ret = get_data() + PACKET_HEADER_SIZE;
		if (okey > 0) ret += 4;
		if (id > 0) ret += 8;
		return ret;
	}

	int get_content_size()
	{
		int ret = size - PACKET_HEADER_SIZE;
		if (okey > 0) ret -= 4;
		if (id > 0) ret -= 8;
		return ret;
	}

	template<class ProtoMessage>
	static message_ptr pb_encode(ProtoMessage & pmessage, int type, uint64_t message_id = 0, uint32_t okey = 0);

	template<class ProtoMessage>
	static bool pb_decode(ProtoMessage & pmessage, message_ptr const & message);

};

class BufferMessage: public Message
{
private:
	char* data;
public:
	BufferMessage(int size, int type, uint64_t message_id = 0, uint32_t okey = 0)
	{
		this->okey = okey;
		this->id = message_id;
		this->size = size;
		this->type = type;
		this->data = (char*) malloc(size + 8);
	}

	virtual ~BufferMessage()
	{
		if (data) free(data);
	}

	char* get_data()
	{
		return data;
	}
};

template<class ProtoMessage>
message_ptr Message::pb_encode(ProtoMessage& pmessage, int type, uint64_t message_id, uint32_t okey)
{
	std::string data = pmessage.SerializeAsString();
	int size = data.size() + PACKET_HEADER_SIZE;
	if (message_id > 0) size += 8;
	if (okey > 0) size += 4;
	message_ptr ret = boost::make_shared<BufferMessage>(size, type);
	unsigned char* buf = (unsigned char*) ret->get_data();
	buf[0] = PACKET_MAGIC;
	buf[1] = PACKET_MAGIC;

	buf[2] = ((size >> 8) & 0xFF);
	buf[3] = (size & 0xFF);

	buf[4] = ((type >> 16) & 0xFF);
	buf[5] = ((type >> 8) & 0xFF);
	buf[6] = (type & 0xFF);
	buf[7] = 0;

	int offset = PACKET_HEADER_SIZE;

	if (okey > 0)
	{
		buf[7] += 1;
		buf[offset++] = ((okey >> 24) & 0xFF);
		buf[offset++] = ((okey >> 16) & 0xFF);
		buf[offset++] = ((okey >> 8) & 0xFF);
		buf[offset++] = (okey & 0xFF);
	}

	if (message_id > 0)
	{
		buf[7] += 2;
		buf[offset++] = ((message_id >> 56) & 0xFF);
		buf[offset++] = ((message_id >> 48) & 0xFF);
		buf[offset++] = ((message_id >> 40) & 0xFF);
		buf[offset++] = ((message_id >> 32) & 0xFF);
		buf[offset++] = ((message_id >> 24) & 0xFF);
		buf[offset++] = ((message_id >> 16) & 0xFF);
		buf[offset++] = ((message_id >> 8) & 0xFF);
		buf[offset++] = (message_id & 0xFF);
	}

	memcpy(buf + offset, data.c_str(), data.size());
	return ret;
}

template<class ProtoMessage>
bool Message::pb_decode(ProtoMessage& pmessage, message_ptr const & message)
{
	return pmessage.ParseFromArray((void*) (message->get_content_data()), message->get_content_size());
}

} /* namespace core */
#endif /* MESSAGE_H_ */
