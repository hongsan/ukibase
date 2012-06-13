/*
 * ObjectService.h
 *
 *  Created on: Apr 19, 2012
 *      Author: hongsan
 */

#ifndef OBJECTSERVICE_H_
#define OBJECTSERVICE_H_

#include "../Database.h"
#include "Service.h"
using namespace ukicore;

namespace ukibase
{

class ObjectService: public Service
{
private:
	Database* database;
public:
	ObjectService();
	virtual ~ObjectService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void get(message_ptr const & message, connection_ptr const & connection);
	void set(message_ptr const & message, connection_ptr const & connection);
	void del(message_ptr const & message, connection_ptr const & connection);
	void inc(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace db */
#endif /* OBJECTSERVICE_H_ */
