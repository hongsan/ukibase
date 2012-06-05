/*
 * Component.h
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_
#include "string"

namespace ukicore
{

class Component
{
private:
	std::string id;
public:
	Component(std::string id)
	{
		this->id = id;
	}

	virtual ~Component()
	{
	}

	std::string get_id()
	{
		return id;
	}

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
};

}

#endif /* COMPONENT_H_ */

