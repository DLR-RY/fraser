/*
 * Copyright (c) 2017-2019, German Aerospace Center (DLR)
 *
 * This file is part of the development version of FRASER.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017-2019, Annika Ofenloch (DLR RY-AVS)
 */

#include "Publisher.h"

#include <iostream>
#include <memory>

#define REQUEST_RETRIES     5

Publisher::Publisher(zmq::context_t & ctx) :
		mZMQcontext(ctx), mZMQpublisher(mZMQcontext, ZMQ_PUB), mZMQSyncResponse(
				mZMQcontext, ZMQ_REP)
{
	// Prepare our context and publisher
	preparePublisher();
}

Publisher::~Publisher()
{
	mZMQpublisher.close();
	mZMQSyncResponse.close();
}

void Publisher::preparePublisher()
{
	mZMQpublisher.setsockopt(ZMQ_LINGER, 1000);
}

bool Publisher::bindSocket(std::string port)
{
	if (!port.empty())
	{
		mZMQpublisher.bind("tcp://*:" + port);
		return true;
	} else
	{
		std::cout << "Could not bind to port" << std::endl;
		return false;
	}
}

bool Publisher::preparePubSynchronization(std::string port)
{
	mZMQSyncResponse.setsockopt(ZMQ_RCVTIMEO, 500);
	mZMQSyncResponse.setsockopt(ZMQ_SNDTIMEO, 500);
	mZMQSyncResponse.setsockopt(ZMQ_LINGER, 1000);

	try
	{
		mZMQSyncResponse.bind("tcp://*:" + port);
	} catch (std::exception &e)
	{
		std::cout << "Could not connect to synchronization service: "
				<< e.what() << std::endl;
		return false;
	}

	return true;
}

bool Publisher::synchronizePub(uint64_t expectedSubscribers,
		uint64_t currentSimTime)
{
	//  Synchronization with subscribers
	uint64_t subscribers = 0;
	uint16_t retries = REQUEST_RETRIES;
	std::string reply = "";

	s_send(mZMQpublisher, "hello");

	while (subscribers < expectedSubscribers)
	{
		//  Poll socket for a reply, with timeout
		if (s_recv2(mZMQSyncResponse, reply))
		{

			s_send(mZMQSyncResponse, reply + "_is_synchronized");
			subscribers++;

		} else
		{

			this->publishEvent("LogWarning", 0,
					"publisher received invalid or no message");

			s_send(mZMQpublisher, "hello");

			if (--retries == 0)
			{
				this->publishEvent("LogError", 0,
						"pub/sub synchronization failed, abandoning…");

				return false;
			}
		}
	}

	return true;
}

void Publisher::publishEvent(std::string identifier, int timestamp,
		std::string data, event::Priority priority, int repeat, int period)
{

	if (data.empty())
	{
		mFbb.Finish(
				event::CreateEvent(mFbb, mFbb.CreateString(identifier),
						timestamp, priority, repeat, period));
	} else
	{
		flexbuffers::Builder flexbuffer;
		flexbuffer.Add(data);
		flexbuffer.Finish();
		auto stringData = mFbb.CreateVector(flexbuffer.GetBuffer());

		mFbb.Finish(
				event::CreateEvent(mFbb, mFbb.CreateString(identifier),
						timestamp, priority, repeat, period, stringData));
	}

	// Envelope
	zmq::message_t envelopeName(identifier.size());
	memcpy(envelopeName.data(), identifier.data(), identifier.size());
	mZMQpublisher.send(envelopeName, ZMQ_SNDMORE);

	// Event
	zmq::message_t event(mFbb.GetSize());
	memcpy((void *) event.data(), mFbb.GetBufferPointer(), mFbb.GetSize());
	mZMQpublisher.send(event);
}

void Publisher::publishEvent(std::string identifier, int timestamp,
		flexbuffers::Builder data, event::Priority priority, int repeat,
		int period)
{

	auto flexData = mFbb.CreateVector(data.GetBuffer());
	mFbb.Finish(
			event::CreateEvent(mFbb, mFbb.CreateString(identifier), timestamp,
					priority, repeat, period, flexData));

	// Envelope
	zmq::message_t envelopeName(identifier.size());
	memcpy(envelopeName.data(), identifier.data(), identifier.size());
	mZMQpublisher.send(envelopeName, ZMQ_SNDMORE);

	// Event
	zmq::message_t event(mFbb.GetSize());
	memcpy((void *) event.data(), mFbb.GetBufferPointer(), mFbb.GetSize());
	mZMQpublisher.send(event);
}
