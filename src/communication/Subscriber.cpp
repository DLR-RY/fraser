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

#define SYNC_TIMEOUT     4000    //  msecs, (> 1000!)

Subscriber::Subscriber(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQsubscriber(mZMQcontext, ZMQ_SUB), mZMQSyncService(
				mZMQcontext, ZMQ_REQ), mEventBuffer(nullptr) {

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

	try {
		mZMQSyncService.connect("tcp://" + ip + ":" + port);
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

bool Subscriber::synchronizeSub() {
	// send a synchronization request
	s_send(mZMQSyncService, "");

	bool expectReply = true;
	while (expectReply) {
		//  Poll socket for a reply, with timeout
		zmq::pollitem_t items[] = { { mZMQSyncService, 0, ZMQ_POLLIN, 0 } };
		zmq::poll(&items[0], 1, SYNC_TIMEOUT);

		//  If we got a reply, process it
		if (items[0].revents & ZMQ_POLLIN) {
			//  We got a reply from the server, must match sequence
			s_recv(mZMQSyncService);

			expectReply = false;
		} else {
			std::cout << mOwner << ": no response after synchronization request"
					<< std::endl;
			return false;
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

		return true;
	} else {
		return false;
	}
}

