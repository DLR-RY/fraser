/*
 * PCDUModel.cpp
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#include <iostream>
#include "Model_1.h"

static const char BREAKPNTS_PATH[] = "../src/subcomponents/model_1/savepoints/";
static const char FILE_EXTENTION[] = "_savefile_model_1.xml";
static const char CONFIG_DIR[] = "../src/subcomponents/model_1/configuration/";
static const char CONFIG_PATH[] =
		"../src/subcomponents/model_1/configuration/config.xml";

Model1::Model1(std::string name, std::string description) :
		mName(name), mDescription(description), mCtx(1), mSubscriber(mCtx), mPublisher(
				mCtx), mDealer(mCtx, mName), mRun(true), mCurrentSimTime(0) {

	mRun = this->prepare();
}

Model1::~Model1() {
}

void Model1::configure(std::string filename) {
	// Load config-file
	// Set values for the fields
	this->restore(filename);

	// Set or calculate other parameters ...
}

bool Model1::prepare() {
	mSubscriber.setOwnershipName(mName);

	boost::filesystem::path dir1(BREAKPNTS_PATH);

	if (!boost::filesystem::exists(dir1)) {
		boost::filesystem::create_directory(dir1);
		std::cout << "Create savepoints-directory for Model1" << "\n";
	}

	boost::filesystem::path dir2(CONFIG_DIR);
	if (!boost::filesystem::exists(dir2)) {
		boost::filesystem::create_directory(dir2);
		std::cout << "Create config-directory for Model1" << "\n";
	}

	if (!mPublisher.bindSocket(mDealer.getPortNumFrom(mName))) {
		return false;
	}

	if (!mSubscriber.connectToPub(mDealer.getIPFrom("simulation_model"),
			mDealer.getPortNumFrom("simulation_model"))) {
		return false;
	}

	if (!mSubscriber.connectToPub(mDealer.getIPFrom("event_queue"),
			mDealer.getPortNumFrom("event_queue"))) {
		return false;
	}

	if (!mSubscriber.connectToPub(mDealer.getIPFrom("model_2"),
			mDealer.getPortNumFrom("model_2"))) {
		return false;
	}

	mSubscriber.subscribeTo(Event("FirstEvent"));
	mSubscriber.subscribeTo(Event("ReturnEvent"));
	mSubscriber.subscribeTo(Event("Restore"));
	mSubscriber.subscribeTo(Event("Store"));
	mSubscriber.subscribeTo(Event("End"));
	mSubscriber.subscribeTo(Event("PCDUCommand"));
	mSubscriber.subscribeTo(Event("CreateDefaultConfigFiles"));
	mSubscriber.subscribeTo(Event("Configure"));

	// Synchronization
	if (!mSubscriber.prepareSubSynchronization(
			mDealer.getIPFrom("simulation_model"),
			mDealer.getSynchronizationPort())) {
		return false;
	}

	if (!mSubscriber.synchronizeSub()) {
		return false;
	}

	return true;
}

void Model1::run() {
	while (mRun) {
		mSubscriber.receiveEvent();
		this->handleEvent();
	}
}

void Model1::handleEvent() {
	mReceivedEvent = mSubscriber.getEvent();
	mEventName = mReceivedEvent.getName();
	mCurrentSimTime = mReceivedEvent.getTimestamp();

	mRun = !foundCriticalSimCycle(mCurrentSimTime);

	if (mEventName == "FirstEvent") {
		this->mPublisher.publishEvent(
				Event("SubsequentEvent", mCurrentSimTime));
	}

	else if (mEventName == "ReturnEvent") {

	}

	else if (mEventName == "CreateDefaultConfigFiles") {
		this->store(CONFIG_PATH);
	}

	else if (mEventName == "Configure") {
		this->configure(CONFIG_PATH);
	}

	else if (mEventName == "Store" || mEventName == "Restore") {
		std::string filename = BREAKPNTS_PATH
				+ mReceivedEvent.getTimestampAsString() + FILE_EXTENTION;

		if (mEventName == "Store") {
			std::cout << "Store events from Queue" << std::endl;
			this->store(filename);

		} else {
			std::cout << "Restore events from Queue" << std::endl;
			this->restore(filename);
		}
	}

	else if (mEventName == "End") {
		mRun = false;
	}

}

void Model1::store(std::string filename) {
	// Store states
	std::ofstream ofs(filename);
	boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);
	try {
		oa << boost::serialization::make_nvp("FieldSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during serializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	}

	mRun = mSubscriber.synchronizeSub();
}

void Model1::restore(std::string filename) {
	// Restore states
	std::ifstream ifs(filename);
	boost::archive::xml_iarchive ia(ifs, boost::archive::no_header);
	try {
		ia >> boost::serialization::make_nvp("FieldSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	}

	mRun = mSubscriber.synchronizeSub();
}
