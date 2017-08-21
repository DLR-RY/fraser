/*
 * Scheduler.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: user
 */

#include "Scheduler.h"

Scheduler::Scheduler() {

}

Scheduler::~Scheduler() {
}

void Scheduler::scheduleEvents(EventSet& eventList) {
	std::sort(eventList.begin(), eventList.end(),
			[](Event const &a, Event const &b) {return a.priorityKey() > b.priorityKey();});
}

