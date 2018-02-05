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

#ifndef QUEUES_SCHEDULER_H_
#define QUEUES_SCHEDULER_H_

#include <vector>
#include <algorithm>

#include "data-types/Event.h"
#include "data-types/EventSet.h"


class Scheduler {
public:
	Scheduler();
	virtual ~Scheduler();

	//void scheduleEvents(std::vector<Event>& eventList);
	void scheduleEvents(EventSet& eventList);
};

#endif /* QUEUES_SCHEDULER_H_ */
