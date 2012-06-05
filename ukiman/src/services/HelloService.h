/*
 * HelloService.h
 *
 *  Created on: Jun 5, 2012
 *      Author: nhsan
 */

#ifndef HELLOSERVICE_H_
#define HELLOSERVICE_H_
#include "Service.h"

using namespace ukicore;
namespace ukiman
{

class HelloService: public Service
{
public:
	HelloService();
	virtual ~HelloService();
	bool process(message_ptr const & message, connection_ptr const & connection);
};

} /* namespace ukibase */
#endif /* HELLOSERVICE_H_ */
