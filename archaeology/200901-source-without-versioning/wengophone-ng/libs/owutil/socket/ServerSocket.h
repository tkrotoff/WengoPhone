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

#include <socket/owsocketdll.h>

#include <QtCore/QThread>

#include <string>

/**
 * Listen to a socket emit a signal
 * when a request arrive.
 *
 * @author Julien Bossart
 * @author Mathieu Stute
 */
class OWSOCKET_API ServerSocket : public QThread {
	Q_OBJECT
public:

	//To finish
	enum Error {
		/** No error. */
		NoError,

		/** Unknow error. */
		UnknownError
	};

	/**
	 * Default constructor.
	 *
	 * Provides a server that listen to the given port.
	 * Emit connectionEvent when a client connect to the socket.
	 *
	 * @param listeningIp listening ip
	 * @param port listening port
	 */
	ServerSocket(const std::string & listeningIp, int port);

	~ServerSocket();

	/**
	 * Starts the thread.
	 */
	void init();

	/**
	 * Sends data to a connection.
	 *
	 * @param connectionId client connection id
	 * @param data data to be written on the socket
	 * @return true if writing has succeeded
	 */
	bool writeToClient(const std::string & connectionId, const std::string & data);

Q_SIGNALS:

	/**
	 * Server status event.
	 *
	 * @param sender this class
	 * @param error error status
	 */
	void serverStatusSignal(Error error);

	/**
	 * A client connection has been established.
	 *
	 * @param sender this class
	 * @param connectionId client connection id
	 */
	void connectionSignal(const std::string & connectionId);

	/**
	 * An incoming request was received.
	 *
	 * @param sender this class
	 * @param connectionId client connection id
	 * @param data received data
	 */
	void incomingRequestSignal(const std::string & connectionId, const std::string & data);

	/**
	 * Write status event.
	 *
	 * @param sender this class
	 * @param writeId write id
	 * @param error error status
	 */
	void writeStatusSignal(const std::string & writeId, Error error);
	
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
