/*
 * Collection.h
 *
 *  Created on: Jun 9, 2012
 *      Author: nhsan
 */

#ifndef COLLECTION_H_
#define COLLECTION_H_
#include <vector>
#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>


using namespace std;

namespace dbclient
{

class Database;

class Collection
{
private:
	string key;
	uint32_t key_type;
	Database* database;
public:
	int get(const string key, string& value);
	int put(const string key, const string value);
	int exist(const string key);
	int del(const string key);

	Collection();
	virtual ~Collection();
};

typedef boost::shared_ptr<Collection> collection_ptr;

} /* namespace dbclient */
#endif /* COLLECTION_H_ */
