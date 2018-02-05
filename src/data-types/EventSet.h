/*
 * Copyright (c) 2017-2018, German Aerospace Center (DLR)
 *
 * This file is part of the development version of FRASER.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017-2018, Annika Ofenloch (DLR RY-AVS)
 */

#ifndef EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_EVENTSET_H_
#define EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_EVENTSET_H_

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

#include "Event.h"

class EventSet: public std::vector<Event> {
public:
	EventSet() :
			mSize(0) {
	}

	virtual ~EventSet() {
	}

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int) {
		if (Archive::is_loading::value) {
			this->clear();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (int i = 0; i < mSize; i++) {
				Event event;
				ar & boost::serialization::make_nvp("Event", event);
				this->push_back(event);
			}
		} else {
			mSize = this->size();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (int i = 0; i < mSize; i++) {
				ar & boost::serialization::make_nvp("Event", this->at(i));
			}
		}
	}

	int mSize; // You have to add vector size
};

//BOOST_CLASS_VERSION(EventSet, 0);
//BOOST_CLASS_IMPLEMENTATION(EventSet, boost::serialization::object_serializable)

#endif /* EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_EVENTSET_H_ */
