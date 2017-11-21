/*
 * Publisher.cpp
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#include "Publisher.h"

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

bool Publisher::synchronizePub(uint64_t expectedSubscribers,
		uint64_t currentSimTime) {
	//  Get synchronization from subscribers

	uint64_t subscribers = 0;
	uint64_t counter = 0;
	while (subscribers < expectedSubscribers) {
		this->publishEvent("Hello", currentSimTime);

		for (uint64_t i = 0; i < expectedSubscribers; i++) {
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

//template<class T>
void Publisher::publishEvent(std::string name, uint64_t timestamp,
		event::Priority priority, uint32_t repeat, uint32_t period,
		event::EventData dataType, std::string data) {
	std::cout << "Publish " << name << " with simTime = "
			<< std::to_string(timestamp) << std::endl;

	// Envelope
	zmq::message_t envelopeName(name.size());
	memcpy(envelopeName.data(), name.data(), name.size());
	bool rc = mZMQpublisher.send(envelopeName, ZMQ_SNDMORE);

	// Flatbuffer
	flatbuffers::FlatBufferBuilder fbb;
	flatbuffers::Offset<void> dataOffset = 0;
	if (dataType == event::EventData_String) {
		dataOffset = fbb.CreateString(data).Union();
	} else if (dataType == event::EventData_NONE) {
		// Do Nothing
	} else {
		// Do Nothing
	}

	auto eventOffset = event::CreateEvent(fbb, fbb.CreateString(name),
			timestamp, priority, repeat, period, dataType, dataOffset);
	fbb.Finish(eventOffset);

	// Event
	int buffersize = fbb.GetSize();
	zmq::message_t event(buffersize);
	memcpy((void *) event.data(), fbb.GetBufferPointer(), buffersize);
	rc = mZMQpublisher.send(event);
}

void Publisher::publishEventSet(std::vector<Event> eventSet) {
//	for (auto& event : eventSet) {
//		s_sendmore(mZMQpublisher, event.getName().c_str());
//		fb_send(mZMQpublisher, event);
//	}
}
