/*
 * ListService.h
 *
 *  Created on: May 13, 2012
 *      Author: hongsan
 */

#ifndef LISTSERVICE_H_
#define LISTSERVICE_H_
#include "../Database.h"
#include "Service.h"
using namespace ukicore;

namespace ukibase
{

class ListService: public Service
{
private:
	Database* database;
public:
	ListService();
	virtual ~ListService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void create(message_ptr const & message, connection_ptr const & connection);
	void destroy(message_ptr const & message, connection_ptr const & connection);
	void push_back(message_ptr const & message, connection_ptr const & connection);
	void push_front(message_ptr const & message, connection_ptr const & connection);
	void size(message_ptr const & message, connection_ptr const & connection);
	void index(message_ptr const & message, connection_ptr const & connection);
	void remove(message_ptr const & message, connection_ptr const & connection);
	void range(message_ptr const & message, connection_ptr const & connection);
	void clear(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace core */
#endif /* LISTSERVICE_H_ */
