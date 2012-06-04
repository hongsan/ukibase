/*
 * ConsistentHash.cpp
 *
 *  Created on: Feb 24, 2012
 *      Author: nhsan
 */

#include "HashRing.h"
#include <glog/logging.h>
#include <boost/lexical_cast.hpp>
#include "commons/HashUtil.h"

#define U32_MASK 0xFFFFFFFF

namespace ukicore
{

HashRing::HashRing(int replicas)
{
	this->number_of_replicas = replicas;
}

HashRing::~HashRing()
{
}

uint32_t HashRing::node_hash_code(servernode_ptr node)
{
	return HashUtil::hash_code(node->get_name().c_str());
}

bool HashRing::add_node(servernode_ptr node)
{
	boost::mutex::scoped_lock lock(mutex);
	uint32_t nHash = node_hash_code(node);
	for (int i = 1; i <= number_of_replicas; ++i)
	{
		uint64_t ullHash = (uint64_t) (nHash * RAND_MUL_FACTOR * i);
		uint32_t uHash = (ullHash & U32_MASK);
		if (nodes.count(uHash) != 0)
		{
			DLOG(INFO) << "Hash Collision!" << i << "-th of total " << number_of_replicas << " replica not added.";
		}
		else
		{
			nodes[uHash] = node;
		}
	}
	return true;
}

bool HashRing::del_node(servernode_ptr node)
{
	boost::mutex::scoped_lock lock(mutex);
	uint32_t nHash = node_hash_code(node);
	for (int i = 1; i <= number_of_replicas; ++i)
	{
		uint64_t ullHash = (uint64_t) (nHash * RAND_MUL_FACTOR * i);
		uint32_t uHash = (ullHash & U32_MASK);
		if (nodes.count(uHash) != 0)
		{
			if (nodes[uHash] == node) nodes.erase(uHash);
			else DLOG(INFO) << "Hash collision when removing bucket.";
		}
		else
		{
			DLOG(INFO) << "The bucket is not currently associated in this hash function.";
		}
	}
	return true;
}

servernode_ptr HashRing::get_node(const char* id)
{
	uint32_t hash = HashUtil::hash_code(id);
	return get_node(hash);
}

servernode_ptr HashRing::get_node(uint32_t hash)
{
	boost::mutex::scoped_lock lock(mutex);
	servernode_ptr pRet;

	if (nodes.count(hash) != 0)
	{
		pRet = nodes[hash];
	}
	else
	{
		std::map<uint32_t, servernode_ptr>::const_iterator iter = nodes.lower_bound(hash);
		if (iter == nodes.end())
		{
			pRet = nodes.begin()->second;
		}
		else
		{
			pRet = iter->second;
		}
	}
	return pRet;
}

void HashRing::build_preferences()
{
	boost::mutex::scoped_lock lock(mutex);
	for (std::map<uint32_t, servernode_ptr>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		servernode_ptr node = it->second;
		std::map<uint32_t, servernode_ptr>::const_iterator it1 = nodes.find(it->first);

		while (it1->second == node)
		{
			if (it1 == nodes.begin()) it1 = nodes.end();
			it1--;
		}
		node->preference = it1->second;
	}
}

void HashRing::del_node_and_build_preferences(servernode_ptr node)
{
	boost::mutex::scoped_lock lock(mutex);
	uint32_t nHash = node_hash_code(node);
	for (int i = 1; i <= number_of_replicas; ++i)
	{
		uint64_t ullHash = (uint64_t) (nHash * RAND_MUL_FACTOR * i);
		uint32_t uHash = (ullHash & U32_MASK);
		if (nodes.count(uHash) != 0)
		{
			if (nodes[uHash] == node) nodes.erase(uHash);
			else DLOG(INFO) << "Hash collision when removing bucket.";
		}
		else
		{
			DLOG(INFO) << "The bucket is not currently associated in this hash function.";
		}
	}

	for (std::map<uint32_t, servernode_ptr>::const_iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		servernode_ptr node = it->second;
		std::map<uint32_t, servernode_ptr>::const_iterator it1 = nodes.find(it->first);

		while (it1->second == node)
		{
			if (it1 == nodes.begin()) it1 = nodes.end();
			it1--;
		}
		node->preference = it1->second;
	}
}

} /* namespace core */
