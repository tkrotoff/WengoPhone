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

#include <socket/ServerSocket.h>
#include <util/Event.h>

class WengoPhone;

// work around a f*c*i*g VS 2003 bug that produces an INTERNAL COMPILER ERROR.
typedef struct contact_info_s {
	std::string wengoName;
	std::string sip;
	std::string firstname;
	std::string lastname;
	std::string country;
	std::string city;
	std::string state;
	std::string group;
} contact_info_t;
////

/**
 * Command server. Listen for local connections,
 * implements a simple command protocol.
 * @see http://
 *
 * @author Mathieu Stute
 */
class CommandServer {
public:

	// work around a f*c*i*g VS 2003 bug that produces an INTERNAL COMPILER ERROR.
	/**
	 * @param sender this class.
	 * @param wengoName the wengoName of the contact.
	 */
	/*
	Event<void (const std::string & wengoName,
		const std::string & sip, const std::string & firstname,
		const std::string & lastname, const std::string & country,
		const std::string & city, const std::string & state,
		const std::string & group)> showAddContactEvent;
	*/
	Event<void (contact_info_t contactInfo)> showAddContactEvent;
	////

	CommandServer(WengoPhone & wengoPhone);

	~CommandServer();

	static CommandServer & getInstance(WengoPhone & wengoPhone);

private:

	static std::string buildHttpForFlash(const std::string & xml);

	/**
	 * @see ServerSocket::serverStatusEvent
	 */
	void serverStatusEventHandler(ServerSocket & sender, ServerSocket::Error error);

	/**
	 * @see ServerSocket::connectionEvent
	 */
	void connectionEventHandler(ServerSocket & sender, const std::string & connectionId);

	/**
	 * @see ServerSocket::incomingRequestEvent
	 */
	void incomingRequestEventHandler(ServerSocket & sender, const std::string & connectionId, const std::string & data);

	/**
	 * @see ServerSocket::writeStatusEvent
	 */
	void writeStatusEventHandler(ServerSocket & sender, const std::string & writeId, ServerSocket::Error error);

	ServerSocket * _serverSocket;

	static CommandServer * _commandServerInstance;

	static const int _port = 25902;

	static const std::string _queryStatus;

	static const std::string _queryCall;

	static const std::string _querySms;

	static const std::string _queryChat;

	static const std::string _queryAddContact;

	WengoPhone & _wengoPhone;
};

#endif	//OWCOMMANDSERVER_H
