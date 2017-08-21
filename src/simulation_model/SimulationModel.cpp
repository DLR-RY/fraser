/*
 * SimulationModel.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: Annika Ofenloch
 */

#include "SimulationModel.h"

#include <iostream>

static const char BREAKPNTS_PATH[] = "../src/simulation_model/savepoints/";
static const char FILE_EXTENTION[] = "_savefile_simulation.xml";
static const char CONFIG_DIR[] = "../src/simulation_model/configuration/";
static const char CONFIG_FILE[] = "../src/simulation_model/configuration/config.xml";

SimulationModel::SimulationModel(std::string name, std::string description) :
		mName(name), mDescription(description), mCtx(1), mPublisher(mCtx), mDealer(
				mCtx, mName), mSimTime("SimTime", 1000), mSimTimeStep(
				"SimTimeStep", 100), mCurrentSimTime("CurrentSimTime", 0), mCycleTime(
				"CylceTime", 0), mSpeedFactor("SpeedFactor", 1.0) {

	registerInterruptSignal();

	mRun = this->prepare();
}

SimulationModel::~SimulationModel() {
}

void SimulationModel::configure(std::string filename) {
	if (mRun) {
		mLoadConfigFile = true;
		this->restore(filename);

		mCycleTime.setValue(
				double(mSimTimeStep.getValue()) / mSpeedFactor.getValue()); // Wait-time between the cycles in milliseconds
	}
}

bool SimulationModel::prepare() {
	boost::filesystem::path dir1(BREAKPNTS_PATH);
	if (!boost::filesystem::exists(dir1)) {
		boost::filesystem::create_directory(dir1);
		std::cout << "Create savepoints-directory for SimulationModel" << "\n";
	}

	boost::filesystem::path dir2(CONFIG_DIR);
	if (!boost::filesystem::exists(dir2)) {
		boost::filesystem::create_directory(dir2);
		std::cout << "Create config-directory for SimulationModel" << "\n";
	}

	mTotalNumOfModels = mDealer.getTotalNumberOfModels();
	mNumOfPersistModels = mDealer.getNumberOfPersistModels();

	if (!mPublisher.bindSocket(mDealer.getPortNumFrom(mName))) {
		return false;
	}

	// Synchronization
	if (!mPublisher.preparePubSynchronization(
			mDealer.getSynchronizationPort())) {
		return false;
	}

	// (mTotalNumOfModels - 1), because the simulation models itself should not be included
	if (!mPublisher.synchronizePub(mTotalNumOfModels - 1,
			mCurrentSimTime.getValue())) {
		return false;
	}

	return true;
}

void SimulationModel::run() {

	if (mConfigMode) {
		this->store(CONFIG_FILE);
		mRun = false;
	}

	int currentSimTime = mCurrentSimTime.getValue();
	if (mRun) {
		while (currentSimTime <= mSimTime.getValue()) {
			if (!mPause) {

				for (auto breakpoint : getBreakpoints()) {
					if (currentSimTime == breakpoint) {
						std::string filename = BREAKPNTS_PATH
								+ std::to_string(breakpoint) + FILE_EXTENTION;

						this->store(filename);
						break;
					}

				}

				std::cout << "[SIMTIME] --> " << currentSimTime << std::endl;
				mPublisher.publishEvent(
						Event("SimTimeChanged", currentSimTime));

				std::this_thread::sleep_for(
						std::chrono::milliseconds(mCycleTime.getValue()));

				currentSimTime += mSimTimeStep.getValue();
				mCurrentSimTime.setValue(currentSimTime);

			}

			if (interruptOccured) {
				break;
			}
		}
	}

	// Stop all running models and the dns server
	mPublisher.publishEvent(Event("End", currentSimTime));
	mDealer.stopDNSserver();
}

void SimulationModel::restore(std::string filename) {
	this->pauseSim();

	// Restore states
	std::ifstream ifs(filename);
	boost::archive::xml_iarchive ia(ifs, boost::archive::no_header);
	try {
		ia >> boost::serialization::make_nvp("FieldSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	}

	if (mLoadConfigFile) {
		mPublisher.publishEvent(Event("Configure", mCurrentSimTime.getValue()));
	} else {
		mPublisher.publishEvent(Event("Restore", mCurrentSimTime.getValue()));
	}

	// Synchronization is necessary, because the simulation
	// has to wait until the other models finished their Restore-method
	// (mNumOfPersistModels - 1), because the simulation model itself should not be included
	mRun = mPublisher.synchronizePub(mNumOfPersistModels - 1,
			mCurrentSimTime.getValue());

	this->continueSim();
}

void SimulationModel::store(std::string filename) {
	this->pauseSim();

	if (mConfigMode) {
		mPublisher.publishEvent(
				Event("CreateDefaultConfigFiles", mCurrentSimTime.getValue()));
	} else {
		mPublisher.publishEvent(Event("Store", mCurrentSimTime.getValue()));
	}

	// Store states
	std::ofstream ofs(filename);
	boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);

	try {
		oa << boost::serialization::make_nvp("FieldSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during serializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	}

	//if (!mConfigMode) {
	// Synchronization is necessary, because the simulation
	// has to wait until the other models finished their Store-method
	// (mNumOfPersistModels - 1), because the simulation model itself should not be included
	mRun = mPublisher.synchronizePub(mNumOfPersistModels - 1,
			mCurrentSimTime.getValue());
	//}

	this->continueSim();
}
