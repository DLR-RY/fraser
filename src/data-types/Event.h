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

#ifndef DATA_TYPES_EVENT_H_
#define DATA_TYPES_EVENT_H_

#include <iostream>
#include <string>
#include <chrono>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

enum class Priority
	: int {
		NORMAL_PRIORITY = 0, HIGH_PRIORITY = 1
};

class Event {
public:

	Event() :
			mName(""), mTimestamp(0), mData(0), mPeriod(0), mRepeat(0), mPriority(
					Priority::NORMAL_PRIORITY) {
	}

	// Overloaded Constructor
	Event(std::string name, uint32_t data, int timestamp = -1, int period = 0, int repeat = 0,
			Priority priority = Priority::NORMAL_PRIORITY) :
			mName(name), mData(data), mTimestamp(timestamp), mPeriod(period), mRepeat(repeat), mPriority(priority) {
	}

	// Overloaded Constructor
	Event(std::string name, int timestamp = -1, int period = 0, int repeat = 0,
			Priority priority = Priority::NORMAL_PRIORITY) :
			mName(name), mTimestamp(timestamp), mData(0), mPeriod(period), mRepeat(
					repeat), mPriority(priority) {
	}

	Event& operator=(const Event* other) {
		mName = other->mName;
		mTimestamp = other->mTimestamp;
		mPeriod = other->mPeriod;
		mRepeat = other->mRepeat;
		mPriority = other->mPriority;
		mData = other->mData;

		return *this;
	}

	bool operator==(const Event* other) {
		if (mName == other->mName && mTimestamp == other->mTimestamp
				&& mPeriod == other->mPeriod && mRepeat == other->mRepeat
				&& mPriority == other->mPriority && mData == other->mData) {

			return true;
		}

		else {
			return false;
		}
	}

	bool operator!=(const Event* other) {
		return !(*this == other);
	}

	virtual ~Event() {
	}

	std::string getName() {
		return mName;
	}

	std::string getTimestampAsString() {
		return std::to_string(mTimestamp);
	}

	void setCurrentSimTime(int milliseconds) {
		mTimestamp = milliseconds;
	}

	uint32_t getData() {
		return mData;
	}

	int priorityKey() const {
		return (mTimestamp - static_cast<int>(mPriority));
	}

	int getPeriod() const {
		return mPeriod;
	}

	void setPeriod(int period) {
		mPeriod = period;
	}

	int getRepeat() const {
		return mRepeat;
	}

	void setRepeat(int repeat) {
		mRepeat = repeat;
	}

	uint64_t getTimestamp() const {
		return mTimestamp;
	}

	void setTimestamp(int timestamp) {
		mTimestamp = timestamp;
	}

private:
	friend class boost::serialization::access;

	template<typename Archive>
	void serialize(Archive& archive, const unsigned int) {
		archive & boost::serialization::make_nvp("Name", mName);
		archive & boost::serialization::make_nvp("Timestamp", mTimestamp);
		archive & boost::serialization::make_nvp("Period", mPeriod);
		archive & boost::serialization::make_nvp("Repeat", mRepeat);
		archive & boost::serialization::make_nvp("Priority", mPriority);
		archive & boost::serialization::make_nvp("Data", mData);
	}

	std::string mName;
	uint32_t mData;
	uint64_t mTimestamp;

	int mPeriod;
	int mRepeat;
	Priority mPriority;

};

BOOST_CLASS_IMPLEMENTATION(Event, boost::serialization::object_serializable)

#endif /* DATA_TYPES_EVENT_H_ */
