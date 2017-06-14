/*
 * Queue.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: Annika Ofenloch
 */

#include "Queue.h"

#include <iostream>

static const char BREAKPNTS_PATH[] = "../src/queues/event_queue_1/savepoints/";
static const char FILE_EXTENTION[] = "_savefile_queue.xml";
static const char CONFIG_DIR[] = "../src/queues/event_queue_1/configuration/";
static const char CONFIG_PATH[] =
		"../src/queues/event_queue_1/configuration/config.xml";

Queue::Queue(std::string name, std::string description) :
		mName(name), mDescription(description), mCtx(1), mSubscriber(mCtx), mPublisher(
				mCtx), mDealer(mCtx, mName), mCurrentSimTime(-1) {

	registerInterruptSignal();

	this->setDefaultEvents();
	mRun = this->prepare();
}

Queue::~Queue() {

}

void Queue::configure(std::string filename) {
	this->restore(filename);
}

void Queue::setDefaultEvents() {
	// For the performance test
	mEventSet.push_back(Event("FirstEvent", 0));

	mScheduler.scheduleEvents(mEventSet);
}

bool Queue::prepare() {
	mSubscriber.setOwnershipName(mName);

	boost::filesystem::path dir1(BREAKPNTS_PATH);
	if (!boost::filesystem::exists(dir1)) {
		boost::filesystem::create_directory(dir1);
		std::cout << "Create savepoints-directory for Queue" << "\n";
	}

	boost::filesystem::path dir2(CONFIG_DIR);
	if (!boost::filesystem::exists(dir2)) {
		boost::filesystem::create_directory(dir2);
		std::cout << "Create config-directory for Queue" << "\n";
	}

	if (!mPublisher.bindSocket(mDealer.getPortNumFrom(mName))) {
		return false;
	}

	if (!mSubscriber.connectToPub(mDealer.getIPFrom("simulation_model"),
			mDealer.getPortNumFrom("simulation_model"))) {
		return false;
	}

	mSubscriber.subscribeTo(Event("SimTimeChanged"));
	mSubscriber.subscribeTo(Event("End"));
	mSubscriber.subscribeTo(Event("Restore"));
	mSubscriber.subscribeTo(Event("Store"));
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

void Queue::run() {
	while (mRun) {
		mSubscriber.receiveEvent();
		this->handleEvent();

		if (interruptOccured) {
			break;
		}
	}
}

void Queue::updateEvents() {

	if (mEventSet.back().getRepeat() != 0) {
		int timestamp = mCurrentSimTime + mEventSet.back().getPeriod();
		mEventSet.back().set_timestamp(timestamp);

		if (mEventSet.back().getRepeat() != -1) {
			mEventSet.back().setRepeat(mEventSet.back().getRepeat() - 1);
		}

		mScheduler.scheduleEvents(mEventSet);

	} else {
		mEventSet.pop_back();
	}
}

void Queue::handleEvent() {
	mReceivedEvent = mSubscriber.getEvent();
	mEventName = mSubscriber.getEventName();
	mCurrentSimTime = mReceivedEvent.getTimestamp();

	if (mEventName == "SimTimeChanged") {

		if (!mEventSet.empty()) {
			Event next_event = mEventSet.back();

			if (mCurrentSimTime >= next_event.getTimestamp()) {
				next_event.setCurrentSimTime(mCurrentSimTime);
				mPublisher.publishEvent(next_event);
				this->updateEvents();
			}
		}
	}

	else if (mEventName == "CreateDefaultConfigFiles") {
		this->store(CONFIG_PATH);
	}

	else if (mEventName == "Configure") {
		this->restore(CONFIG_PATH);
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
		std::cout << "Queue: End-Event" << std::endl;
		mRun = false;
	}

}

void Queue::store(std::string filename) {
	// Store states
	std::ofstream ofs(filename);
	boost::archive::xml_oarchive oa(ofs, boost::archive::no_header);

	try {
		oa << boost::serialization::make_nvp("EventSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during serializing: " << std::endl;
		throw ex.what();
	}

	mRun = mSubscriber.synchronizeSub();
}

void Queue::restore(std::string filename) {
	// Restore states
	std::ifstream ifs(filename);
	boost::archive::xml_iarchive ia(ifs, boost::archive::no_header);

	try {
		ia >> boost::serialization::make_nvp("EventSet", *this);

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		throw ex.what();
	}

	mScheduler.scheduleEvents(mEventSet);

	mRun = mSubscriber.synchronizeSub();
}
