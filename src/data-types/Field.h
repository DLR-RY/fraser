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

#ifndef DATA_TYPES_FIELD_H_
#define DATA_TYPES_FIELD_H_

#include <iostream>
#include <string>
#include <vector>
#include <boost/serialization/serialization.hpp>

template<typename T>
class Field
{
public:
	// Default Constructor
	Field()
	{
	}

	Field(std::string name, T value) :
			mName(name), mValue(value)
	{
	}

	~Field()
	{
	}

	const std::string& getName() const
	{
		return mName;
	}

	void setName(const std::string& name)
	{
		mName = name;
	}

	T getValue() const
	{
		return mValue;
	}

	void setValue(T value)
	{
		mValue = value;
	}

private:

	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& archive, const unsigned int)
	{
		archive & boost::serialization::make_nvp("Name", mName);
		archive & boost::serialization::make_nvp("Value", mValue);
	}

	std::string mName;
	T mValue;

};

BOOST_CLASS_IMPLEMENTATION( Field<int>, boost::serialization::object_serializable )
BOOST_CLASS_IMPLEMENTATION( Field<double>, boost::serialization::object_serializable )
BOOST_CLASS_IMPLEMENTATION( Field<bool>, boost::serialization::object_serializable )
BOOST_CLASS_IMPLEMENTATION( Field<float>, boost::serialization::object_serializable )
BOOST_CLASS_IMPLEMENTATION( Field<std::string>, boost::serialization::object_serializable )

#endif /* DATA_TYPES_FIELD_H_ */
