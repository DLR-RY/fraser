/*
 * main.cpp
 *
 *  Created on: Jan 03, 2017
 *      Author: Annika Ofenloch
 */

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <zmq.hpp>

#include "Queue.h"

int main() {
	Queue eventQueue("event_queue", "Queue includes the scheduled events.");
	try {
		eventQueue.run();

	} catch (zmq::error_t& e) {
		std::cout << "QueueModel: Interrupt received: Exit" << std::endl;
	}

	return 0;
}

