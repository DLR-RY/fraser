/*
 * PCDUModel.h
 *
 *  Created on: Dec 29, 2016
 *      Author: Annika Ofenloch
 */

#ifndef MODEL_1_MODEL_1_H_
#define MODEL_1_MODEL_1_H_

#include <fstream>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/filesystem.hpp>

#include <zmq.hpp>
#include <helper_classes/zhelpers.hpp>

#include "communication/Subscriber.h"
#include "communication/Publisher.h"
#include "communication/Dealer.h"
#include "interfaces/IModel.h"
#include "interfaces/IPersist.h"
#include "data_types/Field.h"

class Model1: public virtual IModel,
		public virtual IPersist {
public:
	Model1(std::string name, std::string description);
	virtual ~Model1();

	// IModel
	virtual void configure(std::string filename) override;
	virtual bool prepare() override;
	virtual void run() override;

	virtual std::string getName() const override {
		return mName;
	}
	virtual std::string getDescription() const override {
		return mDescription;
	}

	// IPersist
	void store(std::string filename);
	void restore(std::string filename);

private:
	// IModel
	std::string mName;
	std::string mDescription;

	// Subscriber
	void handleEvent();
	zmq::context_t mCtx;
	Subscriber mSubscriber;
	Publisher mPublisher;
	Dealer mDealer;

	bool mRun;
	Event mReceivedEvent;
	std::string mEventName;
	int mCurrentSimTime;

	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& archive, const unsigned int) {

	}
};

//BOOST_CLASS_VERSION(Model1, 0);
//BOOST_CLASS_IMPLEMENTATION(Model1, boost::serialization::object_serializable)

#endif
