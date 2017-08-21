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
#include "interfaces/IModel.h"
#include <pugixml.hpp>
#include <string>

//  This is our external configuration server, which deals with requests and sends the requested IP or Port back to the client.
//  The server can handle one request at time.

class ConfigurationServer: public virtual IModel {
public:
	ConfigurationServer();
	virtual ~ConfigurationServer();

	// IModel
	virtual void configure(std::string /*filename*/) override {
	}
	virtual bool prepare() override;
	virtual void run() override;
	virtual std::string getName() const override {
		return mName;
	}
	virtual std::string getDescription() const override {
		return mDescription;
	}

	// Request Methods
	int getNumberOfModels();
	int getNumberOfPersistModels();
	std::vector<std::string> getModelNames();
	std::string getModelInformation(std::string request);

	// Get informations from xml-file
	void setMinAndMaxPort();

	// Set Port numbers
	bool setModelPortNumbers();

	// Set IP addresses
	void setModelIPAddresses();

private:
	// IModel
	std::string mName;
	std::string mDescription;

	zmq::context_t mCtx;
	zmq::socket_t mFrontend;

	pugi::xml_node mRootNode;
	pugi::xml_document mDocument;

	bool mRun = true;
	int mMinPort = 0;
	int mMaxPort = 0;

	std::vector<std::string> mModelNames;
	std::map<std::string, std::string> mModelInformation;
};

#endif /* CONFIGURATION_SERVER_CONFIGURATIONSERVER_H_ */
