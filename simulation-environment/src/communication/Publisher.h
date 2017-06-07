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

#include "helper_classes/zhelpers.hpp"
#include "data_types/Event.h"

class Publisher {
public:
	Publisher(zmq::context_t & ctx);
	virtual ~Publisher();

	// forbid copying
	Publisher(Publisher const &) = delete;
	Publisher & operator=(Publisher const &) = delete;

	bool bindSocket(std::string port);
	bool preparePubSynchronization(std::string port);
	bool synchronizePub(int expectedSubscribers, int currentSimTime);

	void publishEvent(Event event);
	void publishEventSet(std::vector<Event> eventSet);

private:
	void preparePublisher();

	zmq::context_t &mZMQcontext;
	zmq::socket_t mZMQpublisher;
	zmq::socket_t mZMQSyncService;
};

#endif /* INTERFACES_PUBLISHER_H_ */
