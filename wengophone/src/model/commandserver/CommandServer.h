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

#ifndef OWCOMMANDSERVER_H
#define OWCOMMANDSERVER_H

#include <localserver/LocalServer.h>

class WengoPhone;

/**
 * Command server. Listen for local connections,
 * implements a simple command protocol.
 * @see http://
 *
 * @author Mathieu Stute
 */
class CommandServer {
public:

	CommandServer(WengoPhone & wengoPhone);

	~CommandServer();

	static CommandServer & getInstance(WengoPhone & wengoPhone);

	/**
	 * Server status event handler.
	 *
	 * @param sender sender of the event.
	 * @param error error status.
	 */
	void serverStatusEventHandler(LocalServer * sender, LocalServer::Error error);

	/**
	 * A client connection has been established handler.
	 *
	 * @param sender sender of the event.
	 * @param connectionId client connection id.
	 */
	void connectionEventHandler(LocalServer * sender, const std::string & connectionId);

	/**
	 * An incoming request was received handler.
	 *
	 * @param sender sender of the event.
	 * @param connectionId client connection id.
	 * @param data received data.
	 */
	void incomingRequestEventHandler(LocalServer * sender, const std::string & connectionId, const std::string & data);

	/**
	 * Write status event.
	 *
	 * @param sender sender of the event.
	 * @param writeId write id.
	 * @param error error status
	 */
	void writeStatusEventHandler(LocalServer * sender, const std::string & writeId, LocalServer::Error error);

private:

	static std::string buildHttpForFlash(std::string xml);

	LocalServer * _localServer;

	static CommandServer * _commandServerInstance;

	static int _port;

	static std::string _queryStatus;

	static std::string _queryCall;

	static std::string _querySms;

	WengoPhone & _wengoPhone;
};

#endif //OWCOMMANDSERVER_H
