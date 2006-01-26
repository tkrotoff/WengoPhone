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

#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include <model/imwrapper/IMChat.h>

#include <NonCopyable.h>
#include <Event.h>

#include <map>

class IMAccount;
class Chat;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ChatHandler : NonCopyable {
public:

	/**
	 * @see IMChat::messageReceivedEvent
	 */
	Event<void (IMChat & sender, int session, const std::string & from, const std::string & message)> messageReceivedEvent;

	/**
	 * @see IMChat::messageStatusEvent
	 */
	Event<void (IMChat & sender, int session, IMChat::StatusMessage status, const std::string & message)> statusMessageEvent;

	ChatHandler();

	~ChatHandler();

	/**
	 * Create a new session.
	 *
	 * @param imAccount IMAccount that will be associated with the new session
	 * @return id of the new session
	 */
	int createSession(const IMAccount & imAccount);

	/**
	 * Close a session.
	 *
	 * @param session the SessionChat id to delete
	 */
	void closeSession(int session);

	/**
	 * Send a message to all Contact linked to the session.
	 *
	 * @param session the session to send the message to
	 * @param message the message to send
	 */
	void sendMessage(int session, const std::string & message);

	/**
	 * Add a contact to the session.
	 *
	 * @param session the session id
	 * @param contactId the identifier of the contact
	 */
	void addContact(int session, const std::string & contactId);

	/**
	 * Remove a contact from the session.
	 *
	 * @param session the session id
	 * @param contactId the identifier of the contact
	 */
	void removeContact(int session, const std::string & contactId);

	void connected(IMAccount & account);

	void disconnected(IMAccount & account);

private:

	typedef std::map<IMAccount *, Chat *> ChatMap;
	typedef std::map<int, Chat *> SessionChatMap;

	/**
	 * Find the Chat related to the given protocol.
	 *
	 * @param chatMap the ChatMap to search in
	 * @param protocol the protocol
	 * @return an iterator to the desired Chat or 'end' of the given ChatMap
	 */
	static ChatMap::iterator findChat(ChatMap & chatMap, IMAccount & imAccount);

	ChatMap _chatMap;

	SessionChatMap _sessionChatMap;

};

#endif	//CHATHANDLER_H
