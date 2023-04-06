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

#ifndef OWCHATSESSIONMANAGER_H
#define OWCHATSESSIONMANAGER_H

#include <util/Event.h>

#include <vector>

class Account;
class ChatSession;
class IChatSessionManager;
class IChatSession;
class UserProfile;

/**
 * Chat Session Manager.
 *
 * Used to manage classes related to Chat.
 *
 * @author Philippe Bernery
 */
class ChatSessionManager {

	friend class ChatSession;

public:

	/**
	 * Emitted when a new ChatSession is created from outside.
	 *
	 * @param sender this class
	 * @param chatSession pointer to the newly created ChatSession.
	 * This ChatSession must be deleted by the receiver of the Event.
	 */
	Event< void (ChatSessionManager & sender, ChatSession * chatSession)> newChatSessionCreatedEvent;

	ChatSessionManager(UserProfile & userProfile);

	virtual ~ChatSessionManager();

	/**
	 * Creates a ChatSession. Caller is responsible for deleting the
	 * Session.
	 */
	ChatSession * createChatSession();

private:

	/**
	 * @see IChatSessionManager::newIChatSessionCreatedEvent
	 */
	void newIChatSessionCreatedEventHandler(IChatSessionManager & sender, 
		IChatSession * iChatSession);

	/**
	 * Creates a IChatSession compatible with the given Account.
	 *
	 * @return an new instance of an IChatSession implementation or NULL
	 * if no compatible implementation found.
	 */
	IChatSession * createIChatSessionForAccount(const Account & account);

	/** Vector of available ChatSessionManager. */
	std::vector<IChatSessionManager *> _chatSessionManagerVector;

	/** Link to UserProfile. */
	UserProfile & _userProfile;

};

#endif //OWCHATSESSIONMANAGER_H
