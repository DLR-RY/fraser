#ifndef __ZHELPERS_HPP_INCLUDED__
#define __ZHELPERS_HPP_INCLUDED__

#include <zmq.hpp> // https://github.com/zeromq/cppzmq

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../data-types/Event.h"

//  Receive 0MQ string from socket, deserialize it and convert into an Event
static inline Event b_recv(zmq::socket_t & socket) {
	zmq::message_t reply;
	socket.recv(&reply);

	std::string input_data_(static_cast<const char*>(reply.data()),
			reply.size());

	std::stringstream archive_stream(input_data_);
	Event event;

	try {
		boost::archive::text_iarchive ia { archive_stream };
		ia >> event;

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	} catch (int e) {
		std::cout << "EXCEPTION " << e << std::endl;
	}

	return event;
}

//  Receive 0MQ string from socket, deserialize it and convert into an string vector
static inline std::vector<std::string> v_recv(zmq::socket_t & socket) {
	zmq::message_t reply;
	socket.recv(&reply);

	std::string input_data_(static_cast<const char*>(reply.data()),
			reply.size());

	std::stringstream archive_stream(input_data_);
	std::vector<std::string> stringVector;

	try {
		boost::archive::text_iarchive ia { archive_stream };
		ia >> stringVector;

	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during deserializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	} catch (int e) {
		std::cout << "EXCEPTION " << e << std::endl;
	}

	return stringVector;
}

//  Receive 0MQ string from socket and convert into string
static inline std::string s_recv(zmq::socket_t & socket) {

	zmq::message_t message;
	socket.recv(&message);

	return std::string(static_cast<char*>(message.data()), message.size());
}

//  Serialize Event, Convert string to 0MQ string, and send to socket
static inline bool b_send(zmq::socket_t & socket, Event data) {
	std::stringstream ss;
	try {
		boost::archive::text_oarchive oa { ss };
		oa << data;
	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during serializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	} catch (int e) {
		std::cout << "EXCEPTION " << e << std::endl;
	}

	std::string outbound_data_ = ss.str();

	// no need to use the c-style string function 'strlen'
	int len = outbound_data_.length();

	zmq::message_t msgToSend(len);
	memcpy(msgToSend.data(), outbound_data_.c_str(), len);

	bool rc = socket.send(msgToSend);
	return (rc);
}

//  Serialize string vector, Convert string to 0MQ string, and send to socket
static inline bool v_send(zmq::socket_t & socket, std::vector<std::string> stringVector) {
	std::stringstream ss;
	try {
		boost::archive::text_oarchive oa { ss };
		oa << stringVector;
	} catch (boost::archive::archive_exception& ex) {
		std::cout << "Archive Exception during serializing:" << std::endl;
		std::cout << ex.what() << std::endl;
	} catch (int e) {
		std::cout << "EXCEPTION " << e << std::endl;
	}

	std::string outbound_data_ = ss.str();

	// no need to use the c-style string function 'strlen'
	int len = outbound_data_.length();

	zmq::message_t msgToSend(len);
	memcpy(msgToSend.data(), outbound_data_.c_str(), len);

	bool rc = socket.send(msgToSend);
	return (rc);
}

//  Convert string to 0MQ string and send to socket
static inline bool s_send(zmq::socket_t & socket, const std::string & string) {

	zmq::message_t message(string.size());
	memcpy(message.data(), string.data(), string.size());

	bool rc = socket.send(message);
	return (rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
static inline bool s_sendmore(zmq::socket_t & socket,
		const std::string & string) {

	zmq::message_t message(string.size());
	memcpy(message.data(), string.data(), string.size());

	bool rc = socket.send(message, ZMQ_SNDMORE);
	return (rc);
}

#endif
