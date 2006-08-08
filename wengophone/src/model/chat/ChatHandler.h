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

#ifndef OWCHATHANDLER_H
#define OWCHATHANDLER_H

#include <util/Event.h>
#include <util/NonCopyable.h>
#include <util/Trackable.h>

#include <map>
#include <set>

class ConnectHandler;
class IMAccount;
class IMChat;
class IMChatSession;
class IMContactSet;
class UserProfile;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ChatHandler : NonCopyable, public Trackable {
public:

	ChatHandler(UserProfile & userProfile);

	~ChatHandler();

	/**
	 * Emitted when a new IMChatSession has been created.
	 *
	 * @param sender this class
	 * @param imChatSession the new IMChatSession
	 */
	Event<void (ChatHandler & sender, IMChatSession & imChatSession)> newIMChatSessionCreatedEvent;

	/**
	 * Creates a new IMChatSession.
	 * The newChatSessionCreatedEvent is emitted when an IMChatSession is created
	 *
	 * @param imAccount the IMAccount for which we want to create the new IMChatSession
	 */
	void createSession(IMAccount & imAccount, IMContactSet & imContactSet);

private:

	/**
	 * @see ConnectHandler::connectedEvent
	 */
	void connectedEventHandler(ConnectHandler & sender, IMAccount & account);

	/**
	 * @see ConnectHandler::disconnectedEvent
	 */
	void disconnectedEventHandler(ConnectHandler & sender, IMAccount & account);

	/**
	 * @see IMChat::newIMChatSessionCreatedEvent
	 */
	void newIMChatSessionCreatedEventHandler(IMChat & sender, IMChatSession & imChatSession);

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see IMAccount::imAccountDeadEvent
	 */
	void imAccountDeadEventHandler(IMAccount & sender);

	/**
	 * @see IMChatSession::imChatSessionWillDieEvent
	 */
	void imChatSessionWillDieEventHandler(IMChatSession & sender);

	typedef std::map<IMAccount *, IMChat *> IMChatMap;

	IMChatMap _imChatMap;

	typedef std::set<IMChatSession *> IMChatSessionSet;

	IMChatSessionSet _imChatSessionSet;

	UserProfile & _userProfile;
};

#endif	//OWCHATHANDLER_H
