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

using namespace std;
using namespace ukicore;

int main(int argc, char *argv[])
{
        /* Initialize logger */
        google::InitGoogleLogging(argv[0]);
        google::LogToStderr();

        /* Server is singleton */
        Engine& engine = Engine::get_instance();

        /* TODO: register component */

        /* init engine */
        engine.init(argv[1]);
        /* Start the server */
        engine.start();
        return 0;
}


