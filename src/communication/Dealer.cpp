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

#include "Dealer.h"

#define SERVER_TASK_IP std::string("localhost")
#define SERVER_TASK_PORT std::string("5570")

Dealer::Dealer(zmq::context_t & ctx, std::string identity) :
		mZMQcontext(ctx), mReqSocket(mZMQcontext, ZMQ_DEALER) {

	// "The ZMQ_LINGER option shall set the linger period for the specified socket. The linger period determines how long
	// pending messages which have yet to be sent to a peer shall linger in memory after a socket is closed"
	mReqSocket.setsockopt(ZMQ_LINGER, 0);
	mReqSocket.setsockopt(ZMQ_RCVTIMEO, 10000); // timeout in 10 secs
	mReqSocket.setsockopt(ZMQ_IDENTITY, identity.c_str(), identity.size());

	try {
		mReqSocket.connect("tcp://" + SERVER_TASK_IP+ ":" + SERVER_TASK_PORT);
	} catch (zmq::error_t &e) {
		std::cout<<"Could not connect to DNS server"<<std::endl;
	}
}

Dealer::~Dealer() {
	mReqSocket.close();
}

void Dealer::stopDNSserver() {
	s_send(mReqSocket, "End");
}

std::string Dealer::getIPFrom(std::string modelName) {

	std::string request = modelName + "_ip";
	s_send(mReqSocket, request);

	std::string ip = s_recv(mReqSocket);
//	std::cout << "IP: " << ip << std::endl;
	return ip;
}

std::string Dealer::getModelParameter(std::string modelName,
		std::string paramName) {
	std::string request = modelName + "_" + paramName;
	s_send(mReqSocket, request);

	std::string paramValue = s_recv(mReqSocket);
//	std::cout << paramName << ": " << paramValue << std::endl;
	return paramValue;
}

std::string Dealer::getPortNumFrom(std::string modelName) {
	std::string request = modelName + "_port";
	s_send(mReqSocket, request);
	std::string port = s_recv(mReqSocket);
//	std::cout <<modelName <<" bind to Port: " << port << std::endl;
	return port;
}

std::string Dealer::getSynchronizationPort() {
	std::string request = "sim_sync_port";
	s_send(mReqSocket, request);
	std::string port = s_recv(mReqSocket);
//	std::cout << "Port: " << port << std::endl;
	return port;
}

std::vector<std::string> Dealer::getAllModelNames() {
	std::string request = "all_model_names";
	s_send(mReqSocket, request);

	std::vector < std::string > modelNames = v_recv(mReqSocket);
	return modelNames;
}

uint64_t Dealer::getTotalNumberOfModels() {
	std::string request = "total_num_models";
	s_send(mReqSocket, request);
	std::string num = s_recv(mReqSocket);
	//std::cout << num << std::endl;
	return std::stoi(num);
}

uint64_t Dealer::getNumberOfPersistModels() {
	std::string request = "num_persist_models";
	s_send(mReqSocket, request);
	std::string num = s_recv(mReqSocket);
	std::cout << num << std::endl;
	return std::stoi(num);
}

std::vector<std::string> Dealer::getModelDependencies() {
	std::string request = "model_dependencies";
	s_send(mReqSocket, request);

	std::vector < std::string > modelDependencies = v_recv(mReqSocket);
	return modelDependencies;
}
