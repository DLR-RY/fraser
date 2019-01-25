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

//#define SYNC_TIMEOUT     5000    //  msecs, (> 1000!)

Publisher::Publisher(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQpublisher(mZMQcontext, ZMQ_PUB), mZMQSyncRouter(
				mZMQcontext, ZMQ_ROUTER) {
	// Prepare our context and publisher
	preparePublisher();
}

Publisher::~Publisher() {
	mZMQpublisher.close();
	mZMQSyncRouter.close();
}

void Publisher::preparePublisher() {
	mZMQpublisher.setsockopt(ZMQ_LINGER, 500);
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
	mZMQSyncRouter.setsockopt(ZMQ_RCVTIMEO, 1000);
	mZMQSyncRouter.setsockopt(ZMQ_SNDTIMEO, 1000);
	mZMQSyncRouter.setsockopt(ZMQ_LINGER, 500);

	try {
		mZMQSyncRouter.bind("tcp://*:" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	return true;
}

bool Publisher::synchronizePub(uint64_t expectedSubscribers,
		uint64_t currentSimTime) {
	//  Synchronization with subscribers
	uint64_t subscribers = 0;
	uint16_t retries = 5;

	while (subscribers < expectedSubscribers) {

		//  Poll socket for a reply, with timeout
		std::string identity = s_recv(mZMQSyncRouter);
		std::string msg = s_recv(mZMQSyncRouter);

		std::cout << "[Info] simulation model received request " << msg
				<< " by (" << identity << ")" << std::endl;

		if (msg == "sync") {
			s_sendmore(mZMQSyncRouter, identity);
			s_send(mZMQSyncRouter, identity+"_is_synchronized");
			subscribers++;

		} else {
			std::cout << "[Warning] publisher received invalid message"
					<< std::endl;
			if (--retries == 0) {
				std::cout << "[Error] sync failed, abounding" << std::endl;
				return false;
			}
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
