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

#define SYNC_TIMEOUT     1500    //  msecs, (> 1000!)

Publisher::Publisher(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQpublisher(mZMQcontext, ZMQ_PUB), mZMQSyncService(
				mZMQcontext, ZMQ_REP) {
	// Prepare our context and publisher
	preparePublisher();
}

Publisher::~Publisher() {
	mZMQpublisher.close();
	mZMQSyncService.close();
}

void Publisher::preparePublisher() {
	mZMQpublisher.setsockopt(ZMQ_LINGER, 100);
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

	try {
		mZMQSyncService.bind("tcp://*:" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	//  Configure socket to not wait at close time
	int linger = 0;
	mZMQSyncService.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

	return true;
}

bool Publisher::synchronizePub(uint64_t expectedSubscribers,
		uint64_t currentSimTime) {
	//  Synchronization with subscribers
	uint64_t subscribers = 0;
	uint16_t retry = 5;

	while (subscribers < expectedSubscribers) {

		bool expectMessage = true;
		while (expectMessage) {
			//  Poll socket for a reply, with timeout
			zmq::pollitem_t items[] = { { mZMQSyncService, 0, ZMQ_POLLIN, 0 } };
			zmq::poll(&items[0], 1, SYNC_TIMEOUT);

			if (items[0].revents & ZMQ_POLLIN) {
				s_recv(mZMQSyncService);

				expectMessage = false;
			} else {
				if (retry <= 0) {
					std::cout
							<< "W: have not received all replies from the subscribed models"
							<< std::endl;
					return false;
				} else {
					// Retry and send sync message again
					s_send(mZMQSyncService, "");
					retry--;
				}
			}
		}

		s_send(mZMQSyncService, "");

		subscribers++;
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
