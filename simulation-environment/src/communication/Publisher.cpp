/*
 * Publisher.cpp
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#include "../communication/Publisher.h"

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

Publisher::Publisher(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQpublisher(mZMQcontext, ZMQ_PUB), mZMQSyncService(
				mZMQcontext, ZMQ_PULL) {
	// Prepare our context and publisher
	preparePublisher();
}

Publisher::~Publisher() {
	mZMQpublisher.close();
	mZMQSyncService.close();
}

void Publisher::preparePublisher() {
	mZMQpublisher.setsockopt(ZMQ_LINGER, 0);
	// "ZMQ_SNDHWM: Set high water mark for outbound messages"
	mZMQpublisher.setsockopt(ZMQ_SNDHWM, 2000);
}

bool Publisher::bindSocket(std::string port) {
	if (!port.empty()) {
		mZMQpublisher.bind("tcp://*:" + port);
		return true;
	} else {
		std::cout << "Could not bind to port" << std::endl;
		return false;
	}
}

bool Publisher::preparePubSynchronization(std::string port) {
	//  Socket to receive signals
	if (!port.empty()) {
		mZMQSyncService.setsockopt(ZMQ_LINGER, 0);
		mZMQSyncService.setsockopt(ZMQ_RCVTIMEO, 1000); // TimeOut after 1000ms
		mZMQSyncService.bind("tcp://*:" + port);
		return true;
	} else {
		std::cout << "Could not bind to sync port" << std::endl;
		return false;
	}
}

bool Publisher::synchronizePub(int expectedSubscribers, int currentSimTime) {
	//  Get synchronization from subscribers
	int subscribers = 0;
	int counter = 0;
	while (subscribers < expectedSubscribers) {
		this->publishEvent(Event("Hello", currentSimTime));

		for (int i = 0; i < expectedSubscribers; i++) {
			std::string message = s_recv(mZMQSyncService);

			if (message == "OK") {
				//std::cout << "Received OK" << std::endl;
				subscribers++;
			}
		}

		counter++;
		// Send ten "hello"-Event
		if (counter > 10) {
			std::cout
					<< "have not received all replies from the subscribed models"
					<< std::endl;
			return false;
		}

	}

	return true;
}

void Publisher::publishEvent(Event event) {
//	std::cout << "Publish " << event.getName() << " with simTime = "
//			<< event.getTimestamp() << std::endl;
	s_sendmore(mZMQpublisher, event.getName().c_str());
	b_send(mZMQpublisher, event);
}

void Publisher::publishEventSet(std::vector<Event> eventSet) {
	for (auto& event : eventSet) {
		s_sendmore(mZMQpublisher, event.getName().c_str());
		b_send(mZMQpublisher, event);
	}
}
