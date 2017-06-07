/*
 * IPersist.h
 *
 *  Created on: Jan 7, 2017
 *      Author: Annika Ofenloch
 */

#ifndef INTERFACES_IPERSIST_H_
#define INTERFACES_IPERSIST_H_

#include <vector>

class IPersist {
public:
	virtual ~IPersist() {
	}

	/** Store all states of the model and save them in a file. **/
	virtual void store(std::string filename) = 0;
	/** Restore all states of the model from a specific savepoint (breakpoint). **/
	virtual void restore(std::string filename) = 0;

};

#endif /* INTERFACES_IPERSIST_H_ */
