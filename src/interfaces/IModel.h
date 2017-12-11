/*
 * IModel.h
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#ifndef INTERFACES_IMODEL_H_
#define INTERFACES_IMODEL_H_

#include <signal.h>
// Is set by interrupt signal to stop the model
static bool interruptOccured = false;

class IModel {
public:
	virtual ~IModel() {
	}

protected:
	/** Load the configuration-file and
	 * Initialize the parameters with the values of the configuration-file. **/
	virtual void configure(std::string filename) = 0;

	/** Prepare the SUBSCRIBER, PUBLISHER and SYNC sockets.
	 * This method must be called before the run-method.
	 * All models must be connected to the simulation model during the preparation
	 * (for the synchronization & configuration, receiving the simulation time and terminating the model).
	 * In addition, the owner of the subscriber has to be specified by passing the model name.
	 * Returns FALSE if something went wrong. **/
	virtual bool prepare() = 0;

	/** Run the model and start receiving events from the publisher. **/
	virtual void run() = 0;

	/** Get the model name. **/
	virtual std::string getName() const = 0;

	/** Get the model description. **/
	virtual std::string getDescription() const = 0;

	/** Check if events from different simulation cycles are present. **/
	bool foundCriticalSimCycle(uint64_t currentSimTime) {

		if (mCurrentSimCycle == 0) {
			mCurrentSimCycle = currentSimTime;
		} else {
			if (currentSimTime < mCurrentSimCycle) {
				std::cout
						<< "WARNING: Multiple delta cycles are running -> Current simulation time: "
						<< currentSimTime << std::endl;

				return true;
			} else if (currentSimTime > mCurrentSimCycle) {
				mCurrentSimCycle = currentSimTime;
			}
		}

		return false;
	}

	void registerInterruptSignal() {
		// Register interrupt signal
		signal(SIGINT, stopSignal);
	}

	// Capture interrupt signal to stop the model
	static void stopSignal(int /*sig*/) {
		interruptOccured = true;
	}

private:
	uint64_t mCurrentSimCycle = 0;
};

#endif /* INTERFACES_IMODEL_H_ */
