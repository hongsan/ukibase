/*
 * SequenceService.h
 *
 *  Created on: Jun 13, 2012
 *      Author: nhsan
 */

#ifndef SEQUENCESERVICE_H_
#define SEQUENCESERVICE_H_

#include "../Database.h"
#include "Service.h"
using namespace ukicore;

namespace ukibase
{

class SequenceService: public Service
{
private:
	Database* database;
public:
	SequenceService();
	virtual ~SequenceService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void create(message_ptr const & message, connection_ptr const & connection);
	void drop(message_ptr const & message, connection_ptr const & connection);
	void next(message_ptr const & message, connection_ptr const & connection);
	void type(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace ukibase */
#endif /* SEQUENCESERVICE_H_ */
