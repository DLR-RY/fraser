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

	/** Prepare the subscriber and publisher as well as the synchronization.
	 * Prepare-method has to be called before the run-method. Every Model
	 * has to be connected to the simulation-Model for the synchronization and
	 * to receive the End-event to stop the model.
	 * In Addition, set the owner of the Subscriber by passing the model-name.
	 * Configure-method has to be called before the prepare-method.
	 * Return false if something went wrong. **/
	virtual bool prepare() = 0;

	/** Run the model and start to receive events from the publisher. **/
	virtual void run() = 0;

	/** Get name of the model **/
	virtual std::string getName() const = 0;

	/** Get Description of the model **/
	virtual std::string getDescription() const = 0;

	/** Check if events from different simulation cycles are present. **/
	bool foundCriticalSimCycle(int currentSimTime) {

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
	int mCurrentSimCycle = 0;
};

#endif /* INTERFACES_IMODEL_H_ */
