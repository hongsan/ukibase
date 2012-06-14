/*
 * ListService.h
 *
 *  Created on: Jun 14, 2012
 *      Author: nhsan
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
	void drop(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace ukibase */
#endif /* LISTSERVICE_H_ */
