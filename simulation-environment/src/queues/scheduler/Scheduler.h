/*
 * Scheduler.h
 *
 *  Created on: Jan 30, 2017
 *      Author: Annika Ofenloch
 */

#ifndef QUEUES_SCHEDULER_H_
#define QUEUES_SCHEDULER_H_

#include <vector>
#include <algorithm>

#include "data_types/Event.h"
#include "data_types/EventSet.h"


class Scheduler {
public:
	Scheduler();
	virtual ~Scheduler();

	//void scheduleEvents(std::vector<Event>& eventList);
	void scheduleEvents(EventSet& eventList);
};

#endif /* QUEUES_SCHEDULER_H_ */
