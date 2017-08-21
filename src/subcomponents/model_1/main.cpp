/*
 * main.cpp
 *
 *  Created on: Jan 03, 2017
 *      Author: Annika Ofenloch
 */

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <zmq.hpp>

#include "Model_1.h"

int main(int argc, const char * argv[]) {

	Model1 model_1("model_1", "Test Model 1");
	try {
		model_1.run();

	} catch (zmq::error_t& e) {
		std::cout << "Model 1: Interrupt received: Exit" << std::endl;
	}

	return 0;
}

