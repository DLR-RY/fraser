/*
 * main.cpp
 *
 *  Created on: Jan 03, 2017
 *      Author: Annika Ofenloch
 */

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <zmq.hpp>

#include "Model_2.h"

int main() {
	Model2 model2("model_2", "Test Model 2");
	try {
		model2.run();

	} catch (zmq::error_t& e) {
		std::cout << "Model 2: Interrupt received: Exit" << std::endl;
	}

	return 0;
}

