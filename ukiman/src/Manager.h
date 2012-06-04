/*
 * Manager.h
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#ifndef MANAGER_H_
#define MANAGER_H_
#include "Component.h"
#include "Engine.h"

namespace ukicore
{

class Manager: public Component
{
public:
	Manager();
	virtual ~Manager();
	void init();
	void start();
	void stop();
};

} /* namespace ukiman */
#endif /* MANAGER_H_ */
