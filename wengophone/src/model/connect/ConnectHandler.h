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

#ifndef CONNECTHANDLER_H
#define CONNECTHANDLER_H

#include <model/imwrapper/IMConnect.h>
#include <model/chat/ChatHandler.h>

#include <NonCopyable.h>
#include <Event.h>

#include <map>

class Connect;
class PresenceHandler;
class WengoPhone;
class IMAccount;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ConnectHandler : NonCopyable {
public:

	/**
	 * @see IMConnect::loginStatusEvent
	 */
	Event<void (IMConnect & sender, IMConnect::LoginStatus status)> loginStatusEvent;

	/**
	 * Emitted when a IMAccount is connected.
	 * 
	 * @param imAccount the IMAccount that is connected
	 */
	Event<void (ConnectHandler & sender, IMAccount & imAccount)> connectedEvent;

	/**
	 * Emitted when a IMAccount is connected.
	 * 
	 * @param imAccount the IMAccount that is disconnected
	 */
	Event<void (ConnectHandler & sender, IMAccount & imAccount)> disconnectedEvent;

	void connect(const IMAccount & imAccount);

	void disconnect(const IMAccount & imAccount);

	ConnectHandler();

	~ConnectHandler();

private:

	void loginStatusEventHandler(IMConnect & sender, IMConnect::LoginStatus status);

	typedef std::map<const IMAccount *, Connect *> ConnectMap;

	ConnectMap _connectMap;

};

#endif	//CONNECTHANDLER_H
