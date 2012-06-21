/*
 * HashService.h
 *
 *  Created on: Jun 21, 2012
 *      Author: nhsan
 */

#ifndef HASHSERVICE_H_
#define HASHSERVICE_H_
#include "../Database.h"
#include "Service.h"
using namespace ukicore;
namespace ukibase
{

class HashService: public Service
{
private:
	Database* database;
public:
	HashService();
	virtual ~HashService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void create(message_ptr const & message, connection_ptr const & connection);
	void drop(message_ptr const & message, connection_ptr const & connection);
	void size(message_ptr const & message, connection_ptr const & connection);
	void clear(message_ptr const & message, connection_ptr const & connection);
};
}
#endif /* HASHSERVICE_H_ */
