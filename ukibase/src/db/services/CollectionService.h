/*
 * CollectionService.h
 *
 *  Created on: Jun 14, 2012
 *      Author: nhsan
 */

#ifndef COLLECTIONSERVICE_H_
#define COLLECTIONSERVICE_H_
#include "../Database.h"
#include "Service.h"
using namespace ukicore;

namespace ukibase
{

class CollectionService: public Service
{
private:
	Database* database;
public:
	CollectionService();
	virtual ~CollectionService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void create(message_ptr const & message, connection_ptr const & connection);
	void drop(message_ptr const & message, connection_ptr const & connection);
	void id(message_ptr const & message, connection_ptr const & connection);
	void get(message_ptr const & message, connection_ptr const & connection);
	void put(message_ptr const & message, connection_ptr const & connection);
	void del(message_ptr const & message, connection_ptr const & connection);
	void exist(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace ukibase */
#endif /* COLLECTIONSERVICE_H_ */
