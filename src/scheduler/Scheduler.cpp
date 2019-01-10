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

#include "Scheduler.h"

Scheduler::Scheduler() {

}

void Scheduler::scheduleEvents(std::vector<Event>& eventList) {
	std::sort(eventList.begin(), eventList.end(),
			[](Event const &a, Event const &b) {return a.priorityKey() > b.priorityKey();});
}

