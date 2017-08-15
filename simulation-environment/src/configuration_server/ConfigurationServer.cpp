/*
 * ConfigurationServer.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: Annika Ofenloch
 */

#include "ConfigurationServer.h"

ConfigurationServer::ConfigurationServer() :
		mCtx(1), mFrontend(mCtx, ZMQ_ROUTER) {
	std::cout << "ConfigurationServer-Model Constructor" << std::endl;
	registerInterruptSignal();

	mRun = this->prepare();
}

ConfigurationServer::~ConfigurationServer() {
	mFrontend.close();
}

bool ConfigurationServer::prepare() {
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(
			"./src/configuration_server/configuration/models-config.xml");

	if (!result) {
		std::cout << "Error: " << result.description() << std::endl;
		return false;
	} else {

		mRootNode = doc.root();
		return true;
	}
}

int ConfigurationServer::getNumberOfModels() {
	std::string allModelsSearch = "ModelsConfiguration/Models/Model";
	auto xpathAllModels = mRootNode.select_nodes(allModelsSearch.c_str());

	return xpathAllModels.size();
}

int ConfigurationServer::getNumberOfPersistModels() {
	std::string allModelsSearch =
			"ModelsConfiguration/Models/Model[@persist=true]";
	auto xpathAllModels = mRootNode.select_nodes(allModelsSearch.c_str());

	return xpathAllModels.size();
}

std::vector<std::string> ConfigurationServer::getModelNames() {
	std::vector<std::string> modelNames;

	std::string allModelsSearch = "ModelsConfiguration/Models/Model";
	pugi::xpath_node_set xpathAllModels = mRootNode.select_nodes(
			allModelsSearch.c_str());

	if (!xpathAllModels.empty()) {
		for (auto &modelNode : xpathAllModels) {
			modelNames.push_back(modelNode.node().child("Name").text().get());
			std::cout << "ModelName: "
					<< modelNode.node().child("Name").text().get() << std::endl;
		}
	}

	return modelNames;
}

std::string ConfigurationServer::getHostAddressFromModel(
		std::string modelName) {
	// Search for the first matching entry with the given hint attribute
	std::string specificModelSearch = "ModelsConfiguration/Models/Model"; // TODO: Look for a Name

	pugi::xpath_node xpathSpecificModel = mRootNode.select_single_node(
			specificModelSearch.c_str());

	if (xpathSpecificModel) {

		std::string hostID =
				xpathSpecificModel.node().child("HostRef").attribute("hostID").value();

		std::string hostNameSearch = "ModelsConfiguration/Hosts/*[@id=" + hostID
				+ "]/Name";

		pugi::xpath_node xpath_hostName = mRootNode.select_single_node(
				hostNameSearch.c_str());

		std::cout << "HostName: " << xpath_hostName.node().text().get()
				<< std::endl;
	}
}

void ConfigurationServer::run() {

	while (mRun) {
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
			s_send(mFrontend, std::to_string(getNumberOfModels()));
		} else if (msg == "num_persist_models") {
			s_send(mFrontend, std::to_string(getNumberOfPersistModels()));
		} else if (msg == "all_model_names") {
			v_send(mFrontend, getModelNames());
		} else {
			s_send(mFrontend, getHostAddressFromModel(msg));
		}

		if (interruptOccured) {
			break;
		}
	}
}
