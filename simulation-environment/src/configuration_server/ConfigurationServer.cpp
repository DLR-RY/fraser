/*
 * ConfigurationServer.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: Annika Ofenloch
 */

#include "ConfigurationServer.h"

ConfigurationServer::ConfigurationServer() :
		mCtx(1), mFrontend(mCtx, ZMQ_ROUTER), mNumOfModels(0), mNumOfPersistModels(
				0) {
	std::cout << "ConfigurationServer-Model Constructor" << std::endl;

	registerInterruptSignal();

	this->prepare();
}

ConfigurationServer::~ConfigurationServer() {
	mFrontend.close();
}

bool ConfigurationServer::prepare() {
	boost::property_tree::ptree pt;
	boost::property_tree::read_xml("../src/configuration_server/configuration/models-config.xml", pt);

	for (auto& child : pt.get_child("Configuration.Models")) {
		const boost::property_tree::ptree& subchild = child.second;

		mModelNames.push_back(subchild.get<std::string>("Name"));

		if (subchild.count("IP") != 0 && subchild.count("Name") != 0) {
			mHostInfos[subchild.get<std::string>("Name") + "_ip"] =
					subchild.get<std::string>("IP");
		}

		if (subchild.count("Port") != 0 && subchild.count("Name") != 0) {
			mHostInfos[subchild.get<std::string>("Name") + "_port"] =
					subchild.get<std::string>("Port");
		}

		if (subchild.get_child("<xmlattr>.persist").data() == "true") {
			mNumOfPersistModels++;
		}

		mNumOfModels++;
	}

	const boost::property_tree::ptree& child = pt.get_child(
			"Configuration.OtherPorts");

	mFrontendPort = child.get<std::string>("FrontendPort");
	mHostInfos["sim_sync_port"] = child.get<std::string>("SynchronizationPort");

	try {
		mFrontend.bind("tcp://*:" + mFrontendPort);
	} catch (std::exception &e) {
		std::cout << "Could not bind to frontend port of configuration server: " << e.what()
				<< std::endl;
		return false;
	}

	return true;
}

void ConfigurationServer::run() {

	while (true) {
		std::string identity = s_recv(mFrontend);
		//std::cout << "Identity: " << identity << std::endl;
		std::string msg = s_recv(mFrontend);
		//std::cout << "Msg: " << msg << std::endl;

		if (msg == "End") {
			// Stop the DNS server
			break;
		}

		s_sendmore(mFrontend, identity);
		if (msg == "total_num_models") {
			s_send(mFrontend, std::to_string(mNumOfModels));
		} else if (msg == "num_persist_models") {
			s_send(mFrontend, std::to_string(mNumOfPersistModels));
		} else if (msg == "all_model_names") {
			v_send(mFrontend, mModelNames);
		} else {
			s_send(mFrontend, mHostInfos[msg]);
		}

		if(interruptOccured) {
			break;
		}
	}
}
