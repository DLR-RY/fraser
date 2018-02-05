/*
 * Copyright (c) 2017-2018, German Aerospace Center (DLR)
 *
 * This file is part of the development version of FRASER.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017-2018, Annika Ofenloch (DLR RY-AVS)
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
