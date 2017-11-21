/*
 * Publisher.h
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#ifndef INTERFACES_PUBLISHER_H_
#define INTERFACES_PUBLISHER_H_

#include <zmq.hpp>

#include <string>
#include <iostream>
#include <memory>

#include "../../resources/idl/event_generated.h"

#include "zhelpers.hpp"

class Publisher {
public:
	Publisher(zmq::context_t & ctx);
	virtual ~Publisher();

	// forbid copying
	Publisher(Publisher const &) = delete;
	Publisher & operator=(Publisher const &) = delete;

	bool bindSocket(std::string port);
	bool preparePubSynchronization(std::string port);
	bool synchronizePub(uint64_t expectedSubscribers, uint64_t currentSimTime);

	//void publishEvent(Event event);
	//template<class T>
	void publishEvent(std::string name, uint64_t timestamp,
			event::Priority priority = event::Priority_NORMAL_PRIORITY, uint32_t repeat = 0, uint32_t period = 0,
			event::EventData dataType = event::EventData_NONE, std::string data = "");
	void publishEventSet(std::vector<Event> eventSet);

private:
	void preparePublisher();

	zmq::context_t &mZMQcontext;
	zmq::socket_t mZMQpublisher;
	zmq::socket_t mZMQSyncService;
};

#endif /* INTERFACES_PUBLISHER_H_ */
