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

#ifndef INTERFACES_PUBLISHER_H_
#define INTERFACES_PUBLISHER_H_

#include <zmq.hpp>
#include <string>

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

	// With event data
	void publishEvent(std::string identifier, uint8_t *bufferPointer, uint32_t size);

private:
	void preparePublisher();

	zmq::context_t &mZMQcontext;
	zmq::socket_t mZMQpublisher;
	zmq::socket_t mZMQSyncService;
};

#endif /* INTERFACES_PUBLISHER_H_ */
