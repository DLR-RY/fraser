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

#ifndef INTERFACES_IQUEUE_H_
#define INTERFACES_IQUEUE_H_

class IQueue
{
public:
	virtual ~IQueue()
	{
	}

protected:
	/** Check if event is periodic. Update the event-set and schedule them again. */
	virtual void updateEvents() = 0;
};

#endif /* INTERFACES_IQUEUE_H_ */
