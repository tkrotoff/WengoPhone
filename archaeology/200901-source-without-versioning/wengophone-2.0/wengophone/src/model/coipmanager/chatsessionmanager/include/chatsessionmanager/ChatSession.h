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

#ifndef OWCHATSESSION_H
#define OWCHATSESSION_H

#include <chatsessionmanager/IChatSession.h>
#include <coipmanager/Session.h>
#include <imwrapper/IMChat.h>

#include <list>

class ChatSessionManager;

/**
 * 
 *
 * @author Philippe Bernery
 */
class ChatSession : public Session {

	friend class ChatSessionManager;

public:

	/**
	 * Emitted when a message has been received. The message is queued and must
	 * be taken with getReceivedMessage()
	 *
	 * @param sender this class
	 */
	Event<void (ChatSession & sender)> messageReceivedEvent;

	/**
	 * Emitted when a status message has been received. (e.g "Joe is connected")
	 *
	 * @param sender this class
	 * @param status the type of status message
	 * @param message a message describing the status
	 */
	Event<void (ChatSession & sender, IChatSession::StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	/**
	 * Typing state event.
	 *
	 * @param sender this class
	 * @param contactId contact who typing state has changed
	 * @param state @see TypingState
	 */
	Event<void (ChatSession & sender, const IMContact & imContact, IChatSession::TypingState state)> typingStateChangedEvent;

	/**
	 * Emitted when an IMContact has been added in this IMChatSession.
	 *
	 * @param sender this class
	 * @param imContact the added IMContact
	 */
	Event<void (ChatSession & sender, const std::string & contactId)> contactAddedEvent;

	/**
	 * Emitted when an IMContact has been removed.
	 *
	 * @param sender this class
	 * @param imContact the removed IMContact
	 */
	Event<void (ChatSession & sender, const std::string & contactId)> contactRemovedEvent;

	virtual ~ChatSession();

	// Inherited from Session
	virtual void start();

	virtual void stop();

	virtual void pause();

	virtual void resume();

	virtual void addContact(const std::string & contactId);

	virtual void removeContact(const std::string & contactId);

	virtual void setContactList(const StringList & contactList);
	////

	/**
	 * Represents a chat message.
	 */
	class ChatMessage {
	public:

		ChatMessage(const IMContact & imContact, const std::string & message);

		~ChatMessage();

		IMContact getIMContact() const { return _imContact; }

		std::string getMessage() const { return _message; }

	private:

		IMContact _imContact;

		std::string _message;
	};

	/**
	 * Gets the first message queued in ChatSession. Delete it from the queue.
	 *
	 * @return the first message of the queue. NULL if no message in the queue.
	 * The returned IMChatMessage must be freed by the called.
	 */
	ChatMessage * getReceivedMessage();

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
	void setTypingState(IChatSession::TypingState state);

private:

	ChatSession(ChatSessionManager & chatSessionManager, UserProfile & userProfile);

	ChatSession(ChatSessionManager & chatSessionManager, UserProfile & userProfile, 
		IChatSession * iChatSessionImp);	

	/** Running IChatSession. */
	IChatSession * _currentIChatSession;

	typedef std::list<ChatMessage *> ChatMessageList;

	/** List of ChatMessage. */
	ChatMessageList _receivedChatMessageList;

	/** Link to ChatSessionManager. */
	ChatSessionManager & _chatSessionManager;

};

#endif //OWCHATSESSION_H
