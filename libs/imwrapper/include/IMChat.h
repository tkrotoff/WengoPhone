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
class IMContactMap;
class IMChatSession;

/**
 * Wrapper for Instant Messaging chat.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class IMChat : Interface {
	friend class IMChatSession;
	friend class ChatHandler;

public:

	/**
	 * Emitted when a new IMChatSession has been created.
	 * 
	 * @param sender this class
	 * @param imChatSession the new IMChatSession
	 */
	Event<void (IMChat & sender, IMChatSession & imChatSession)> newIMChatSessionCreatedEvent;

	/**
	 * Emitted when a contact has been added to a session
	 * 
	 * @param sender this class
	 * @param imChatSession the session where a contact has been added
	 * @param contactId the added contact
	 */
	Event<void (IMChat & sender, IMChatSession & imChatSession, const std::string & contactId)> contactAddedEvent;

	/**
	 * Emitted when a contact is removed from a session
	 * 
	 * @param sender this class
	 * @param imChatSession the session where a contact has been removed
	 * @param imChatSession the removed contact
	 */
	Event<void (IMChat & sender, IMChatSession & imChatSession, const std::string & contactId)> contactRemovedEvent;

	/**
	 * Chat message received callback.
	 *
	 * @param session the associated IMChatSession.
	 * @param sender this class
	 * @param from message sender
	 * @param message message received
	 */
	Event<void (IMChat & sender, IMChatSession & chatSession, const std::string & from, const std::string & message)> messageReceivedEvent;

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
	 * @param chatSession the associated IMChatSession
	 * @param status new status
	 * @param message @see StatusMessage
	 */
	Event<void (IMChat & sender, IMChatSession & chatSession, StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	virtual ~IMChat() { }

	IMAccount & getIMAccount() const {
		return _imAccount;
	}

	const IMContactMap & getIMContactMap() const {
		return _imContactMap;
	}

protected:

	/**
	 * Create a new IMChatSession.
	 * 
	 * The new IMChatSession is returned by the Event newIMChatSessionCreatedEvent
	 */
	virtual void createSession() = 0;

	/**
	 * Says IMChat to close a new session given a IMChatSession
	 *
	 * @param chatSession the IMChatSession to close
	 */
	virtual void closeSession(IMChatSession & chatSession) = 0;

	/**
	 * Send a message to all Contact linked to the session session.
	 *
	 * @param chatSession the session to send the message to
	 * @param message the message to send
	 */
	virtual void sendMessage(IMChatSession & chatSession, const std::string & message) = 0;

	/**
	 * Add a contact to the session.
	 *
	 * @param chatSession the session id
	 * @param protocol the protocol of the contact to add
	 * @param contactId the identifier of the contact
	 */
	virtual void addContact(IMChatSession & chatSession, const std::string & contactId) = 0;

	/**
	 * Remove a contact from the session.
	 *
	 * @param chatSession the session id
	 * @param protocol the protocol of the contact to add
	 * @param contactId the identifier of the contact
	 */
	virtual void removeContact(IMChatSession & chatSession, const std::string & contactId) = 0;

	IMChat(IMAccount & imAccount, const IMContactMap & imContactMap) 
		: _imAccount(imAccount), _imContactMap(imContactMap) {}

	IMAccount & _imAccount;

	const IMContactMap & _imContactMap;

};

#endif	//IMCHAT_H
