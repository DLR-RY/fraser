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
 * - 2017-2019, Annika Ofenloch (DLR RY-AVS)
 */

#include "Subscriber.h"

#define REQUEST_TIMEOUT     1000    //  msecs, (> 1000!)
#define REQUEST_RETRIES     5

Subscriber::Subscriber(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQsubscriber(mZMQcontext, ZMQ_SUB), mZMQSyncDealer(
				mZMQcontext, ZMQ_DEALER), mEventBuffer(nullptr) {

	// "The ZMQ_LINGER option shall set the linger period for the specified socket. The linger period determines how long
	// pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed"
	mZMQsubscriber.setsockopt(ZMQ_LINGER, 100);
}

Subscriber::~Subscriber() {
	mZMQsubscriber.close();
	mZMQSyncDealer.close();
}

bool Subscriber::connectToPub(std::string ip, std::string port) {
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
	mZMQSyncDealer.setsockopt(ZMQ_RCVTIMEO, 500);
	mZMQSyncDealer.setsockopt(ZMQ_SNDTIMEO, 500);
	mZMQSyncDealer.setsockopt(ZMQ_LINGER, 500);
	mZMQSyncDealer.setsockopt(ZMQ_IDENTITY, mOwner.c_str(), mOwner.size());

	try {
		mZMQSyncDealer.connect("tcp://" + ip + ":" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	return true;
}

bool Subscriber::synchronizeSub() {
	int retries_left = REQUEST_RETRIES;

	std::string request = "sync";
	s_send(mZMQSyncDealer, request);

	while (true) {
		//  We got a reply from the server, must match sequence
		std::string reply = s_recv(mZMQSyncDealer);

		if (reply == (mOwner + "_is_synchronized")) {
			std::cout << "[Info]: publisher replied (" << reply << ")" << std::endl;

			break;
		} else {
			std::cout << "[Error]: malformed reply from server: " << reply
					<< std::endl;
		}

		if (--retries_left == 0) {
			std::cout << "[Error]: server seems to be offline, abandoning"
					<< std::endl;

			return false;

		} else {
			std::cout << "[Warning]: no response from server, retrying…" << std::endl;

			//  Send request again, on new socket
			s_send(mZMQSyncDealer, request);
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

