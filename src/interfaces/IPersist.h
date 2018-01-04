/*
 * IPersist.h
 *
 *  Created on: Jan 7, 2017
 *      Author: Annika Ofenloch
 */

#ifndef INTERFACES_IPERSIST_H_
#define INTERFACES_IPERSIST_H_

class IPersist {
public:
	virtual ~IPersist() {
	}

	/** Save state of the model in a XML file. **/
	virtual void saveState(std::string filename) = 0;

	/** Restore/Load state of the model from a XML file.
	 * Is used for configuring and restoring the model. **/
	virtual void loadState(std::string filename) = 0;

};

#endif /* INTERFACES_IPERSIST_H_ */
