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

#include <model/chat/ChatHandler.h>

#include <imwrapper/IMConnect.h>

#include <util/NonCopyable.h>
#include <util/Event.h>

#include <map>
#include <set>

class Connect;
class PresenceHandler;
class WengoPhone;
class IMAccount;
class UserProfile;

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

	void connect(IMAccount & imAccount);

	void disconnect(IMAccount & imAccount);

	ConnectHandler(UserProfile & userProfile);

	~ConnectHandler();

private:

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see UserProfile::imAccountRemovedEvent
	 */
	void imAccountRemovedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see IMConnect::loginStatusEvent
	 */
	void loginStatusEventHandler(IMConnect & sender, IMConnect::LoginStatus status);

	typedef std::map<IMAccount *, Connect *> ConnectMap;

	typedef std::set<IMAccount> IMAccountSet;

	typedef std::set<IMAccount *> IMAccountPtrSet;

	/**
	 * Find an IMAccount. Testing by value (rather by pointer).
	 */
	static IMAccount * findIMAccount(const IMAccountPtrSet & set, const IMAccount & imAccount);

	ConnectMap _connectMap;

	/**
	 * Used to store connect demands until IMAccount has been
	 * actually creaded. Without this stack, we could have
	 * synchronisation problems with PresencHandler (trying to
	 * change the presence state of an IMAccount whereas it has not
	 * received the newIMAccountAddedEvent.
	 */
	IMAccountSet _pendingConnections;

	/**
	 * IMAccount actually added to WengoPhone. connect could be called
	 * before adding IMAccount. This could leads some problem.
	 */
	IMAccountPtrSet _actualIMAccount;
};

#endif	//CONNECTHANDLER_H
