/*
 * ServerNode.h
 *
 *  Created on: Feb 10, 2012
 *      Author: nhsan
 */

#ifndef SERVERNODE_H_
#define SERVERNODE_H_
#include <string>
#include "Connection.h"
#include <boost/smart_ptr/shared_ptr.hpp>
namespace ukicore
{

enum ServerState
{
	UNKNOWN, CONNECTED, ONLINE, READY, PROBLEM
};

class ServerNode;
typedef boost::shared_ptr<ServerNode> servernode_ptr;

class ServerNode
{
private:
	int id;
	std::string ip;
	int port;
	std::string type;
	std::string name;
public:
	connection_ptr connection;
	ServerState state;
	time_t connected_time;
	servernode_ptr preference;

	ServerNode(int id, std::string name, std::string type, std::string ip, int port)
	{
		this->id = id;
		this->ip = ip;
		this->port = port;
		state = UNKNOWN;
		this->name = name;
	}

	virtual ~ServerNode()
	{
	}

	std::string get_name()
	{
		return name;
	}
	int get_id()
	{
		return id;
	}
	int get_port()
	{
		return port;
	}
	std::string get_ip()
	{
		return ip;
	}
	std::string get_type()
	{
		return type;
	}
};


} /* namespace core */
#endif /* SERVERNODE_H_ */
