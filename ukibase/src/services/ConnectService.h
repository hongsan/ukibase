/*
 * ConnectService.h
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#ifndef CONNECTSERVICE_H_
#define CONNECTSERVICE_H_

#include "Service.h"

using namespace ukicore;

namespace ukibase
{

class ConnectService: public Service
{
public:
	ConnectService();
	virtual ~ConnectService();
	bool process(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace ukibase */
#endif /* CONNECTSERVICE_H_ */
