/*
 * Server.h
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "Component.h"
#include <string>
#include "Connection.h"
#include "ServerNode.h"
#include <google/dense_hash_map>

using namespace std;
using namespace ukicore;
namespace ukibase
{

typedef google::dense_hash_map<int, servernode_ptr> node_map;

class ServerConfig: public Component
{
public:
	string man_host;
	int man_port;
	connection_ptr man_connection;
	uint32_t server_id;
	node_map nodes;
	servernode_ptr me;

	ServerConfig();
	virtual ~ServerConfig();
	void init();
	void start();
	void stop();
};

} /* namespace ukibase */
#endif /* SERVER_H_ */
