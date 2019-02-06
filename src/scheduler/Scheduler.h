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

#ifndef QUEUES_SCHEDULER_H_
#define QUEUES_SCHEDULER_H_

#include <algorithm>
#include <boost/serialization/vector.hpp>

#include "data-types/Event.h"

class Scheduler
{
public:
	Scheduler();
	virtual ~Scheduler() = default;

	void scheduleEvents(std::vector<Event>& eventList);
};

#endif /* SCHEDULER_SCHEDULER_H_ */
