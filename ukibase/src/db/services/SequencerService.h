/*
 * SequencerService.h
 *
 *  Created on: Apr 20, 2012
 *      Author: hongsan
 */

#ifndef SEQUENCERSERVICE_H_
#define SEQUENCERSERVICE_H_

#include "../Database.h"
#include "Service.h"
using namespace ukicore;
namespace ukibase
{

class SequencerService: public Service
{
private:
	Database* database;
public:
	SequencerService();
	virtual ~SequencerService();
	bool process(message_ptr const & message, connection_ptr const & connection);
	void create(message_ptr const & message, connection_ptr const & connection);
	void destroy(message_ptr const & message, connection_ptr const & connection);
	void inc(message_ptr const & message, connection_ptr const & connection);
	void dec(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace db */
#endif /* SEQUENCERSERVICE_H_ */

