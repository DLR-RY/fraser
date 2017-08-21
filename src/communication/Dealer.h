/*
 * Dealer.h
 *
 *  Created on: Jan 16, 2017
 *      Author: Annika Ofenloch
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
	int getTotalNumberOfModels();
	int getNumberOfPersistModels();
	void stopDNSserver();

private:
	zmq::context_t &mZMQcontext;
	zmq::socket_t mReqSocket;
};

#endif /* COMMUNICATION_DEALER_H_ */
