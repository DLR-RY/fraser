/*
 * main.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: Annika Ofenloch
 */

#include <iostream>
#include <string>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>

#include "SimulationModel.h"

SimulationModel simulation("simulation_model", "Simulation Environment");
static const char CONFIG_PATH[] =
		"../src/simulation_model/configuration/config.xml";

void startSimulationThread() {
	try {
		simulation.configure(CONFIG_PATH);

		simulation.run();
	} catch (boost::thread_interrupted&) {
		std::cout << " SimulationModel: Interrupt received: Exit" << std::endl;
	}
}

void createConfigurationFilesThread() {
	try {
		simulation.setConfigMode(true);
		simulation.run();
	} catch (boost::thread_interrupted&) {
		std::cout << " SimulationModel: Interrupt received: Exit" << std::endl;
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		if (static_cast<std::string>(argv[1]) == "config") {
			std::cout << " Configuration Mode ... Create configuration files"
					<< std::endl;
			boost::thread configThread(createConfigurationFilesThread);
			configThread.join();
		} else {
			std::cout << " Invalid argument/s: --help" << std::endl;
		}

	} else {
		std::cout << "--------> Create simulation thread " << std::endl;
		boost::thread simThread(startSimulationThread);
		simThread.join();   // main thread waits for the thread t to finish
	}

	return 0;
}
