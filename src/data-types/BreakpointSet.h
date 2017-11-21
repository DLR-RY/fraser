/*
 * BreakpointSet.h
 *
 *  Created on: Mar 17, 2017
 *      Author: Annika Ofenloch
 */

#ifndef EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_
#define EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>

class BreakpointSet: public std::vector<uint64_t> {
public:
	BreakpointSet() :
		mSize(0) {
	}

	virtual ~BreakpointSet() {
	}

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int) {
		if (Archive::is_loading::value) {
			this->clear();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (uint64_t i = 0; i < mSize; i++) {
				uint64_t breakpoint;
				ar & boost::serialization::make_nvp("Breakpoint", breakpoint);
				this->push_back(breakpoint);
			}
		} else {
			mSize = this->size();
			ar & boost::serialization::make_nvp("Size", mSize);
			for (uint64_t i = 0; i < mSize; i++) {
				ar & boost::serialization::make_nvp("Breakpoint", this->at(i));
			}
		}
	}

	uint64_t mSize; // You have to add vector size
};

//BOOST_CLASS_VERSION(EventSet, 0);
//BOOST_CLASS_IMPLEMENTATION(EventSet, boost::serialization::object_serializable)

#endif /* EUCROPIS_SIM_TRUNK_SRC_DATA_TYPES_BREAKPOINTSET_H_ */
