/*
 * ConsistentHash.h
 *
 *  Created on: Feb 24, 2012
 *      Author: nhsan
 */

#ifndef CONSISTENTHASH_H_
#define CONSISTENTHASH_H_
#include <map>
#include "ServerNode.h"
#include <boost/thread/mutex.hpp>


namespace ukicore
{

class HashRing
{
private:
	int number_of_replicas;
	static const uint32_t RAND_MUL_FACTOR = 314;
	uint32_t node_hash_code(servernode_ptr);
	std::map<uint32_t, servernode_ptr> nodes;
	std::map<uint32_t, servernode_ptr> preferences;
	boost::mutex mutex;
	void build_preferences();
public:
	HashRing(int replicas);
	virtual ~HashRing();

	bool add_node(servernode_ptr node);
	bool del_node(servernode_ptr node);
	servernode_ptr get_node(const char* id);
	servernode_ptr get_node(uint32_t hash);
	servernode_ptr get_node(uint32_t hash, uint32_t& slot);
	servernode_ptr get_preference(uint32_t hash);
};

typedef boost::shared_ptr<HashRing> hashring_ptr;

} /* namespace comserver */
#endif /* CONSISTENTHASH_H_ */
