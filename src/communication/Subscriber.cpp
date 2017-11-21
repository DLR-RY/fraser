/*
 * Subscriber.cpp
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#include "Subscriber.h"

Subscriber::Subscriber(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQsubscriber(mZMQcontext, ZMQ_SUB), mZMQSyncService(
				mZMQcontext, ZMQ_PUSH), mEvent() {

	// "The ZMQ_LINGER option shall set the linger period for the specified socket. The linger period determines how long
	// pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed"
	mZMQsubscriber.setsockopt(ZMQ_LINGER, 0);
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
	mZMQSyncService.setsockopt(ZMQ_LINGER, 0);

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
	// Wait for hello-Event from the publisher
	while (true) {
		receiveEvent();
		if (mEventName == "Hello") {
			//std::cout << "Received Hello" << std::endl;
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
//std::cout << "Subscribe to " << event.getName() << std::endl;
	mZMQsubscriber.setsockopt(ZMQ_SUBSCRIBE, eventName.data(),
			eventName.length());
}

void Subscriber::receiveEvent() {
	//  Read envelope with address
	zmq::message_t envelopeName;
	mZMQsubscriber.recv(&envelopeName);
	mEventName = std::string(static_cast<char*>(envelopeName.data()),
			envelopeName.size());
	//  Read message contents
	zmq::message_t event;
	mZMQsubscriber.recv(&event);

	flatbuffers::FlatBufferBuilder fbb;
	event::EventBuilder event_builder(fbb);

	mEvent = event::GetEvent(event.data());

	std::cout << mOwner << " [" << mEventName << "] "
			<< std::to_string(mEvent->timestamp()) << std::endl;

}

