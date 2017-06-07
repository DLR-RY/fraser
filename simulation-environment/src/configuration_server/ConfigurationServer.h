/*
 * ServerTask.h
 *
 *  Created on: Jan 23, 2017
 *      Author: Annika Ofenloch
 */

#ifndef CONFIGURATION_SERVER_CONFIGURATIONSERVER_H_
#define CONFIGURATION_SERVER_CONFIGURATIONSERVER_H_

#include <map>

#include <zmq.hpp>

#include "helper_classes/zhelpers.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "interfaces/IModel.h"

//  This is our external configuration server, which deals with requests and sends the requested IP or Port back to the client.
//  The server can handle one request at time.

class ConfigurationServer: public virtual IModel {
public:
	ConfigurationServer();
	virtual ~ConfigurationServer();

	// IModel
	virtual void configure(std::string /*filename*/) override {}
	virtual bool prepare() override;
	virtual void run() override;
	virtual std::string getName() const override {
		return mName;
	}
	virtual std::string getDescription() const override {
		return mDescription;
	}

private:
	// IModel
	std::string mName;
	std::string mDescription;

	std::map<std::string, std::string> mHostInfos;

	zmq::context_t mCtx;
	zmq::socket_t mFrontend;

	std::string mFrontendPort;
	std::vector<std::string> mModelNames;
	int mNumOfModels;
	int mNumOfPersistModels;
};

#endif /* CONFIGURATION_SERVER_CONFIGURATIONSERVER_H_ */
