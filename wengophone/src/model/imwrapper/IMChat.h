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

#ifndef IMCHAT_H
#define IMCHAT_H

#include <Interface.h>
#include <Event.h>

#include <string>

class IMAccount;

/**
 * Wrapper for Instant Messaging chat.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class IMChat : Interface {
public:

	/**
	 * Chat message received callback.
	 *
	 * @param session the session id
	 * @param sender this class
	 * @param from message sender
	 * @param message message received
	 */
	Event<void (IMChat & sender, int session, const std::string & from, const std::string & message)> messageReceivedEvent;

	enum StatusMessage {
		/** Chat message has been received. */
		StatusMessageReceived,

		/** Chat message sending error. 'message' contains the one that produces the error */
		StatusMessageError,
		
		/** Information message (e.g "toto has left the chat room"). 'message' contains the information message */
		StatusMessageInfo
	};

	/**
	 * Message status event.
	 *
	 * @param sender this class
	 * @param status new status
	 * @param message @see StatusMessage
	 */
	Event<void (IMChat & sender, int session, StatusMessage status, const std::string & message)> statusMessageEvent;

	virtual ~IMChat() { }

	/**
	 * Create a new session.
	 *
	 * @return id of the new session
	 */
	virtual int createSession() = 0;

	/**
	 * Close a session.
	 *
	 * @param session the session id to delete
	 */
	virtual void closeSession(int session) = 0;

	/**
	 * Send a message to all Contact linked to the session session.
	 *
	 * @param session the session to send the message to
	 * @param message the message to send
	 */
	virtual void sendMessage(int session, const std::string & message) = 0;

	/**
	 * Add a contact to the session.
	 *
	 * @param session the session id
	 * @param protocol the protocol of the contact to add
	 * @param contactId the identifier of the contact
	 */
	virtual void addContact(int session, const std::string & contactId) = 0;

	/**
	 * Remove a contact from the session.
	 *
	 * @param session the session id
	 * @param protocol the protocol of the contact to add
	 * @param contactId the identifier of the contact
	 */
	virtual void removeContact(int session, const std::string & contactId) = 0;

protected:

	IMChat(IMAccount & account) : _account(account) {}

	IMAccount & _account;
};

#endif	//IMCHAT_H
