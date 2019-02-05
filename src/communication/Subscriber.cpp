/*
 * Copyright (c) 2017-2019, German Aerospace Center (DLR)
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

#define REQUEST_RETRIES     5

Subscriber::Subscriber(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQsubscriber(mZMQcontext, ZMQ_SUB), mZMQSyncRequest(
				mZMQcontext, ZMQ_REQ) {

	// "The ZMQ_LINGER option shall set the linger period for the specified socket. The linger period determines how long
	// pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed"
	mZMQsubscriber.setsockopt(ZMQ_LINGER, 1000);
}

Subscriber::~Subscriber() {
	mZMQsubscriber.close();
	mZMQSyncRequest.close();
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
	mZMQSyncRequest.setsockopt(ZMQ_SNDTIMEO, 500);
	mZMQSyncRequest.setsockopt(ZMQ_RCVTIMEO, 500);
	mZMQSyncRequest.setsockopt(ZMQ_LINGER, 1000);

	try {
		mZMQSyncRequest.connect("tcp://" + ip + ":" + port);
	} catch (std::exception &e) {
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	return true;
}

bool Subscriber::synchronizeSub() {
	subscribeTo("hello");
	int retries_left = REQUEST_RETRIES;
	std::string msg = "";

	while (true) {
		msg = s_recv(mZMQsubscriber);

		if (msg == "hello") {

			//  We got a reply from the server, must match sequence
			s_send(mZMQSyncRequest, mOwner);

			std::string reply;
			if (s_recv2(mZMQSyncRequest, reply)) {

				if (reply == (mOwner + "_is_synchronized")) {
					break;
				} else {
					//  Send request again
					s_send(mZMQSyncRequest, mOwner);
				}

			} else if (--retries_left == 0) {
				std::cout
						<< "[Error]: publisher seems to be offline, abandoning"
						<< std::endl;

				return false;

			} else {
				std::cout << "[Warning]: " << mOwner
						<< " got no response from publisher, retrying…"
						<< std::endl;

				//  Send request again
				s_send(mZMQSyncRequest, mOwner);
			}
		}
	}

	unsubscribeFrom("hello");
	return true;
}

void Subscriber::subscribeTo(std::string eventName) {
	mZMQsubscriber.setsockopt(ZMQ_SUBSCRIBE, eventName.data(),
			eventName.size());
}

void Subscriber::unsubscribeFrom(std::string eventName) {
	mZMQsubscriber.setsockopt(ZMQ_UNSUBSCRIBE, eventName.data(),
			eventName.size());
}

bool Subscriber::receiveEvent() {
	zmq::message_t envelopeName;

	//  Read envelope with address
	bool receivedEnvelope = mZMQsubscriber.recv(&envelopeName);
	mEventName = std::string(static_cast<char*>(envelopeName.data()),
			envelopeName.size());

	bool receivedEvent = mZMQsubscriber.recv(&mEventMsg);

	if (receivedEvent && receivedEnvelope) {
		return true;
	} else {
		return false;
	}
}

