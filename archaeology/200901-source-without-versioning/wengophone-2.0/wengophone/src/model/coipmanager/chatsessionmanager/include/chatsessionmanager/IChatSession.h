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

#ifndef OWICHATSESSION_H
#define OWICHATSESSION_H

#include <coipmanager/Session.h>

class IChatSession : public CoIpModule {
public:

	enum StatusMessage {
		/** Chat message has been received. */
		StatusMessageReceived,

		/** Chat message sending error. 'message' contains the one that produces the error. */
		StatusMessageError,

		/** Information message (e.g "toto has left the chat room"). 'message' contains the information message. */
		StatusMessageInfo
	};

	enum TypingState {
		/** No action is done from the contact. */
		TypingStateNotTyping,

		/** The contact is typing. */
		TypingStateTyping,

		/** The contact stops typing momentarily. */
		TypingStateStopTyping
	};

	/**
	 * Emitted when a message has been received. 
	 *
	 * @param sender this class
	 * @param imContact the sender of the message
	 * @param message the received message
	 */
	Event<void (IChatSession & sender, const IMContact & imContact, 
		const std::string & message)> messageReceivedEvent;

	/**
	 * Emitted when a status message has been received. (e.g "Joe is connected")
	 *
	 * @param sender this class
	 * @param status the type of status message
	 * @param message a message describing the status
	 */
	Event<void (IChatSession & sender, StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	/**
	 * Typing state event.
	 *
	 * @param sender this class
	 * @param contactId contact who typing state has changed
	 * @param state @see TypingState
	 */
	Event<void (IChatSession & sender, const IMContact & imContact, TypingState state)> typingStateChangedEvent;

	/**
	 * Emitted when an IMContact has been added in this IMChatSession.
	 *
	 * @param sender this class
	 * @param imContact the added IMContact
	 */
	Event<void (IChatSession & sender, const IMContact & imContact)> contactAddedEvent;

	/**
	 * Emitted when an IMContact has been removed.
	 *
	 * @param sender this class
	 * @param imContact the removed IMContact
	 */
	Event<void (IChatSession & sender, const IMContact & imContact)> contactRemovedEvent;

	IChatSession();

	virtual ~IChatSession();

	// Inherited from Session
	virtual void start();

	virtual void stop();

	virtual void pause();

	virtual void resume();
	////

	/**
	 * Sets the Account to use.
	 */
	void setAccount(const Account * account);

	/**
	 * Adds an IMContact to the IChatSession.
	 */
	void addContact(const IMContact & imContact);

	/**
	 * Removes an IMContact to the IChatSession.
	 */
	void removeContact(const IMContact & imContact);

	/**
	 * Sends a message to all Contacts in this Session.
	 *
	 * @param message the message to send.
	 */
	void sendMessage(const std::string & message);

	/**
	 * Sets the typing state.
	 *
	 * @param state the desired typing state
	 */
	void setTypingState(TypingState state);

protected:

	/** List of Contacts. */
	std::list<IMContact> _contactList;

	/** Account to use. */
	Account * _account;

};

#endif //OWICHATSESSION_H
