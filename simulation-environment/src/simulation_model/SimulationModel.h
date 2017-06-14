/*
 * SimulationModel.h
 *
 *  Created on: Dec 20, 2016
 *      Author: Annika Ofenloch
 */

#ifndef SIMULATION_MODEL_SIMULATIONMODEL_H_
#define SIMULATION_MODEL_SIMULATIONMODEL_H_

#include <thread>
#include <string>
#include <fstream>
#include <chrono>
#include <zmq.hpp>
#include <boost/thread.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "interfaces/IModel.h"
#include "interfaces/IPersist.h"
#include "communication/Publisher.h"
#include "communication/Dealer.h"
#include "data_types/Field.h"
#include "data_types/BreakpointSet.h"
#include "helper_classes/zhelpers.hpp"

class SimulationModel: public virtual IModel, public virtual IPersist {
public:
	SimulationModel(std::string name, std::string description);

	virtual ~SimulationModel();

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
	virtual void store(std::string filename) override;
	virtual void restore(std::string filename) override;

	void pauseSim() {
		mPause = true;
	}

	void continueSim() {
		mPause = false;
	}

	void setConfigMode(bool status) {
		mConfigMode = status;
	}

	// Properties
	int getCurrentSimTime() {
		return mCurrentSimTime.getValue();
	}

	/** Set a breakpoint (given in simulation time, e.g. 200 time units).
	 * If the simulation reaches a breakpoint (e.g. after 200 time units),
	 * the store method of all subscribed models and of the simulation-model itself is called. **/
	void setBreakpoint(int time) {
		mBreakpoints.push_back(time);
	}

	/** Get all breakpoint which were defined. **/
	std::vector<int> getBreakpoints() {
		return mBreakpoints;
	}

private:
	// IModel
	std::string mName;
	std::string mDescription;

	// For the communication
	zmq::context_t mCtx;  // ZMQ-instance
	Publisher mPublisher; // ZMQ-PUB
	Dealer mDealer;		  // ZMQ-DEALER

	BreakpointSet mBreakpoints;
	bool mRun = true;
	bool mPause = false;
	bool mConfigMode = false;
	bool mLoadConfigFile = false;

	int mTotalNumOfModels = 0;
	int mNumOfPersistModels = 0;

	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& archive, const unsigned int) {
		archive & boost::serialization::make_nvp("IntField", mSimTime);
		archive & boost::serialization::make_nvp("IntField", mSimTimeStep);
		archive & boost::serialization::make_nvp("IntField", mCurrentSimTime);
		archive & boost::serialization::make_nvp("DoubleField", mSpeedFactor);
		archive & boost::serialization::make_nvp("BreakpointSet", mBreakpoints);
	}

	// Fields
	Field<int> mSimTime;
	Field<int> mSimTimeStep;
	Field<int> mCurrentSimTime;
	Field<int> mCycleTime;
	Field<double> mSpeedFactor;
};

//BOOST_CLASS_VERSION(SimulationModel, 0);
//BOOST_CLASS_IMPLEMENTATION(SimulationModel, boost::serialization::object_serializable)

#endif /* SIMULATION_MODEL_SIMULATIONMODEL_H_ */
