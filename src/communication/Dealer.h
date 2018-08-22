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

#ifndef COMMUNICATION_DEALER_H_
#define COMMUNICATION_DEALER_H_

#include <iostream>
#include <zmq.hpp>

#include "zhelpers.hpp"

class Dealer {
public:
	Dealer(zmq::context_t & ctx, std::string identity);
	virtual ~Dealer();

	std::string getIPFrom(std::string modelName);
	std::string getPortNumFrom(std::string modelName);
	std::string getSynchronizationPort();
	std::vector<std::string> getAllModelNames();
	uint64_t getTotalNumberOfModels();
	uint64_t getNumberOfPersistModels();
	std::vector<std::string> getModelDependencies();
	std::string getModelParameter(std::string modelName, std::string paramName);
	void stopDNSserver();

private:
	zmq::context_t &mZMQcontext;
	zmq::socket_t mReqSocket;
};

#endif /* COMMUNICATION_DEALER_H_ */
