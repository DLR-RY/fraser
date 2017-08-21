/*
 * main.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: Annika Ofenloch
 */

#include "ConfigurationServer.h"

//  The main thread simply starts several clients and a server, and then
//  waits for the server to finish.

int main(void) {
	ConfigurationServer configServerModel;
	try {
		configServerModel.run();

	} catch (zmq::error_t& e) {
		std::cout << "ConfigurationServer: Interrupt received: Exit"
				<< std::endl;
	}

	return 0;
}
