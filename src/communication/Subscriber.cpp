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

#include "Subscriber.h"

Subscriber::Subscriber(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQsubscriber(mZMQcontext, ZMQ_SUB), mZMQSyncService(
				mZMQcontext, ZMQ_PUSH), mEventBuffer(nullptr) {

	// "The ZMQ_LINGER option shall set the linger period for the specified socket. The linger period determines how long
	// pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed"
	mZMQsubscriber.setsockopt(ZMQ_LINGER, 100);
}

Subscriber::~Subscriber() {
	mZMQsubscriber.close();
	mZMQSyncService.close();
}

bool Subscriber::connectToPub(std::string ip, std::string port) {
	//  Prepare our context and subscriber

	try {
		mZMQsubscriber.connect("tcp://" + ip + ":" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to Publisher: " << e.what()
				<< std::endl;
		return false;
	}

	return true;
}

bool Subscriber::prepareSubSynchronization(std::string ip, std::string port) {
	mZMQSyncService.setsockopt(ZMQ_LINGER, 100);

	try {
		mZMQSyncService.connect("tcp://" + ip + ":" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	this->subscribeTo("Hello");

	return true;
}

bool Subscriber::synchronizeSub() {
	// Wait for 'Hello'-Event from the publisher
	while (true) {
		mEventName = s_recv(mZMQsubscriber);
		s_recv(mZMQsubscriber);

		if (mEventName == "Hello") {
			s_send(mZMQSyncService, "OK");
			break;
		}
		if (mEventName == "End") {
			return false;
		} else {
			std::cout << "Have not received hello" << std::endl;
		}
	}
	return true;
}

void Subscriber::subscribeTo(std::string eventName) {
	mZMQsubscriber.setsockopt(ZMQ_SUBSCRIBE, eventName.data(),
			eventName.size());
}

bool Subscriber::receiveEvent() {
	bool receivedEnvelope = false;
	bool receivedEvent = false;
	zmq::message_t envelopeName;
	zmq::message_t event;

	//  Read envelope with address
	receivedEnvelope = mZMQsubscriber.recv(&envelopeName);
	mEventName = std::string(static_cast<char*>(envelopeName.data()),
			envelopeName.size());

	receivedEvent = mZMQsubscriber.recv(&event);

	if (receivedEvent && receivedEnvelope) {
		mEventBuffer = event.data();
		mEventName = std::string(static_cast<char*>(envelopeName.data()),
				envelopeName.size());

		return true;
	} else {
		return false;
	}
}

