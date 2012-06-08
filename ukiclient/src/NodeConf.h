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
#include "HashRing.h"

using namespace std;
using namespace ukicore;
namespace dbclient
{

typedef google::dense_hash_map<int, servernode_ptr> node_map;

class NodeConf: public Component
{
public:
	string man_host;
	int man_port;
	connection_ptr man_connection;
	node_map nodes;
	hashring_ptr ring;

	NodeConf();
	virtual ~NodeConf();
	void init();
	void start();
	void stop();
};

} /* namespace ukibase */
#endif /* SERVER_H_ */
