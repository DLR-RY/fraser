/*
 * Subscriber.h
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include <iostream>
#include <zmq.hpp>

#include "zhelpers.hpp"
#include "../../resources/idl/event_generated.h"

class Subscriber {
public:
	Subscriber(zmq::context_t & ctx);
	virtual ~Subscriber();

	// forbid copying
	Subscriber(Subscriber const &) = delete;
	Subscriber & operator=(Subscriber const &) = delete;

	//protected:
	bool connectToPub(std::string ip, std::string port);
	bool prepareSubSynchronization(std::string ip, std::string port);
	bool synchronizeSub();

	void subscribeTo(std::string eventName);

	void receiveEvent();

	std::string getEventName() {
		return mEventName;
	}

	const event::Event* getEvent() {
		return mEvent;
	}

	void setOwnershipName(std::string name) {
		mOwner = name;
	}

	//virtual void handleEvent() = 0;
private:
	zmq::context_t &mZMQcontext;
	zmq::socket_t mZMQsubscriber;
	zmq::socket_t mZMQSyncService;

	std::string mOwner;
	std::string mEventName;
	const event::Event* mEvent;
};

#endif /* SUBSCRIBER_H_ */
