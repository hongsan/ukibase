/*
 * main.cpp
 *
 *  Created on: Jun 4, 2012
 *      Author: nhsan
 */

#include <iostream>
#include <glog/logging.h>
#include "Engine.h"
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include "Manager.h"

using namespace std;
using namespace ukicore;

int main(int argc, char *argv[])
{
        /* Initialize logger */
        google::InitGoogleLogging(argv[0]);
        google::LogToStderr();

        /* Server is singleton */
        Engine& engine = Engine::get_instance();

    	/* register components */
    	component_ptr manager = boost::make_shared<ukiman::Manager>();
    	engine.register_component(manager);

        /* init engine */
        engine.init(argv[1]);
        /* Start the server */
        engine.start();
        return 0;
}


