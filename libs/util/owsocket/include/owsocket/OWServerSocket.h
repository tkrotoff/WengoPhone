/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OWSERVERSOCKET_H
#define OWSERVERSOCKET_H

#include <util/Event.h>
#include <thread/Thread.h>

#include <string>

/**
 * Listen to a socket emit a signal
 * when a request arrive.
 *
 * @author Julien Bossart
 * @author Mathieu Stute
 */
class OWServerSocket : public Thread {
public:

	//To finish
	enum Error {
		/** No error. */
		NoError,

		/** Unknow error. */
		UnknownError
	};

	/**
	 * Server status event.
	 *
	 * @param sender sender of the event.
	 * @param error error status.
	 */
	Event<void (OWServerSocket * sender, Error error)> serverStatusEvent;

	/**
	 * A client connection has been established.
	 *
	 * @param sender sender of the event.
	 * @param connectionId client connection id.
	 */
	Event<void (OWServerSocket * sender, const std::string & connectionId)> connectionEvent;

	/**
	 * An incoming request was received.
	 *
	 * @param sender sender of the event.
	 * @param connectionId client connection id.
	 * @param data received data.
	 */
	Event<void (OWServerSocket * sender, const std::string & connectionId, const std::string & data)> incomingRequestEvent;

	/**
	 * Write status event.
	 *
	 * @param sender sender of the event.
	 * @param writeId write id.
	 * @param error error status
	 */
	Event<void (OWServerSocket * sender, const std::string & writeId, Error error)> writeStatusEvent;

	/**
	 * Default constructor. Provide a server that listen to the given port.
	 * Emit connectionEvent when a client connect to the socket.
	 *
	 * @param listeningIp listening ip.
	 * @param port listening port.
	 */
	OWServerSocket(const std::string & listeningIp, int port);

	/**
	 * Destructor.
	 */
	~OWServerSocket();

	/**
	 * Start the thread.
	 */
	void init();

	/**
	 * Send data to a connection.
	 *
	 * @param connectionId client connection id.
	 * @param data data to be written on the socket.
	 * @return true if writing has succeeded.
	 */
	bool writeToClient(const std::string & connectionId, const std::string & data);

private:

	/**
	 * Thread run method.
	 *
	 * Internal method, this should not be used from outside.
	 */
	void run();

	bool createMainListeningSocket();

	bool checkConnectionId(const std::string & connectionId);

	bool closeAndRemoveFromList(const std::string & connectionId);

	int getRequest(int sockId, char * buff, unsigned buffsize);

	int getHighestSocket();

	bool _started;

	const std::string _listeningIp;

	int _port;
};

#endif	//OWSERVERSOCKET_H
