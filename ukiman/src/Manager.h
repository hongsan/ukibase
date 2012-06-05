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
#include <vector>
#include "ServerNode.h"

using namespace std;
using namespace ukicore;

namespace ukiman
{

typedef vector<servernode_ptr> node_vec;

class Manager: public Component
{
public:
	node_vec nodes;
	Manager();
	virtual ~Manager();
	void init();
	void start();
	void stop();
};

} /* namespace ukiman */
#endif /* MANAGER_H_ */
