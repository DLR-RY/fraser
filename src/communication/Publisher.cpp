/*
 * Copyright (c) 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of FRASER.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017, Annika Ofenloch (DLR RY-AVS)
 */

#include "Publisher.h"

#include <iostream>
#include <memory>

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

bool Publisher::synchronizePub(uint64_t expectedSubscribers,
		uint64_t currentSimTime) {
	//  Get synchronization from subscribers
	uint64_t subscribers = 0;
	uint64_t counter = 0;

	while (subscribers < expectedSubscribers) {
		const std::string eventName = "Hello";
		zmq::message_t envelopeName(eventName.size());
		memcpy(envelopeName.data(), eventName.data(), eventName.size());
		mZMQpublisher.send(envelopeName, ZMQ_SNDMORE);

		const std::string simTime = std::to_string(currentSimTime);
		zmq::message_t message(simTime.size());
		memcpy(message.data(), simTime.data(), simTime.size());
		mZMQpublisher.send(message);

		for (uint64_t i = 0; i < expectedSubscribers; i++) {
			std::string message = s_recv(mZMQSyncService);

			if (message == "OK") {
				std::cout << "Received OK" << std::endl;
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

void Publisher::publishEvent(std::string identifier, uint8_t *bufferPointer,
		uint32_t size) {
	// Envelope
	zmq::message_t envelopeName(identifier.size());
	memcpy(envelopeName.data(), identifier.data(), identifier.size());
	mZMQpublisher.send(envelopeName, ZMQ_SNDMORE);

	// Event
	zmq::message_t event(size);
	memcpy((void *) event.data(), bufferPointer, size);
	mZMQpublisher.send(event);
}
