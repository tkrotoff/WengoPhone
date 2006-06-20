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

#ifndef OWCONNECTHANDLER_H
#define OWCONNECTHANDLER_H

#include <model/chat/ChatHandler.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMConnect.h>

#include <util/Event.h>
#include <util/NonCopyable.h>
#include <util/Trackable.h>

#include <map>
#include <set>

class Connect;
class IMAccount;
class PresenceHandler;
class Thread;
class UserProfile;
class WengoPhone;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ConnectHandler : NonCopyable, public Trackable {
public:

	/**
	 * @see IMConnect::connectedEvent
	 */
	Event<void (ConnectHandler & sender, IMAccount & imAccount)> connectedEvent;

	/**
	 * @see IMConnect::disconnectedEvent
	 */
	Event<void (ConnectHandler & sender, IMAccount & imAccount, bool connectionError, const std::string & reason)> disconnectedEvent;

	/**
	 * @see IMConnect::connectionProgressEvent
	 */
	Event<void (ConnectHandler & sender, IMAccount & imAccount,
		int currentStep, int totalSteps, const std::string & infoMessage)> connectionProgressEvent;

	ConnectHandler(UserProfile & userProfile, Thread & modelThread);

	~ConnectHandler();

	void connect(IMAccount & imAccount);

	void disconnect(IMAccount & imAccount, bool now = false);

private:

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see IMAccount::imAccountDeadEvent
	 */
	void imAccountDeadEventHandler(IMAccount & sender);

	/**
	 * @see IMConnect::connectedEvent
	 */
	void connectedEventHandler(IMConnect & sender);

	/**
	 * Here we must change thread because are sent from Gaim or PhApi Thread.
	 * @see connectedEventHandler
	 */
	void connectedEventHandlerThreadSafe(IMAccount * imAccount);

	/**
	 * @see IMConnect::disconnectedEvent
	 */
	void disconnectedEventHandler(IMConnect & sender, bool connectionError, const std::string & reason);

	/**
	 * Here we must change thread because are sent from Gaim or PhApi Thread.
	 * @see disconnectedEventHandler
	 */
	void disconnectedEventHandlerThreadSafe(IMAccount * imAccount, bool connectionError, std::string reason);

	/**
	 * @see IMConnect::connectionProgressEvent
	 */
	void connectionProgressEventHandler(IMConnect & sender, int currentStep, int totalSteps,
				const std::string & infoMessage);


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

	UserProfile & _userProfile;

	Thread & _modelThread;
};

#endif	//OWCONNECTHANDLER_H
