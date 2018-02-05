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

#ifndef EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_
#define EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

class SavepointSet: public std::vector<uint64_t> {
public:
	SavepointSet() :
			mSize(0) {
	}

	virtual ~SavepointSet() {
	}

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int) {
		if (Archive::is_loading::value) {
			this->clear();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (uint64_t i = 0; i < mSize; i++) {
				uint64_t savepoint;
				ar & boost::serialization::make_nvp("Savepoint", savepoint);
				this->push_back(savepoint);
			}
		} else {
			mSize = this->size();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (uint64_t i = 0; i < mSize; i++) {
				ar & boost::serialization::make_nvp("Savepoint", this->at(i));
			}
		}
	}

	uint64_t mSize; // You have to add vector size
};

//BOOST_CLASS_VERSION(EventSet, 0);
//BOOST_CLASS_IMPLEMENTATION(EventSet, boost::serialization::object_serializable)

#endif /* EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_ */
