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

#ifndef IMCHATSESSION_H
#define IMCHATSESSION_H

#include <imwrapper/IMChat.h>
#include <imwrapper/IMContactSet.h>

#include <string>
#include <set>

class IMAccount;
class IMAccountHandler;
class IMContact;

/**
 * Instant Messaging chat.
 *
 * Example 1 - sending a message:
 * <pre>
 * IMAccount account("bob@hotmail.com", "bob", EnumIMProtocol::IMProtocolMSN);
 * IMConnect connect(account);
 * connect.connect();
 * //Waits that account is connected using IMConnect::LoginStatusConnected
 * IMChat * chat = IMWrapperFactory::getFactory().createIMChat(account);
 * IMChatSession chatSession(*chat);
 * chatSession.addIMContact(IMContact(account, "alice@hotmail.com"));
 * chatSession.addIMContact(IMContact(account, "caroline@hotmail.com"));
 * chatSession.addIMContact(IMContact(account, "tanguy.krotoff@wengo.fr"));
 * chatSession.sendMessage("Hola! how are you?");
 * </pre>
 *
 * Example 2 - sending a message:
 * <pre>
 * IMAccount account("bob@hotmail.com", "bob", EnumIMProtocol::IMProtocolMSN);
 * IMConnect connect(account);
 * connect.connect();
 * //Waits that account is connected using IMConnect::LoginStatusConnected
 * IMChat * chat = IMWrapperFactory::getFactory().createIMChat(account);
 * chat->createSession();
 * //Waits for IMChat::newIMChatSessionCreatedEvent and gets the IMChatSession
 * chatSession.addIMContact(IMContact(account, "alice@hotmail.com"));
 * chatSession.addIMContact(IMContact(account, "caroline@hotmail.com"));
 * chatSession.addIMContact(IMContact(account, "tanguy.krotoff@wengo.fr"));
 * chatSession.sendMessage("Hola! how are you?");
 * </pre>
 *
 * Example 3 - receiving a message:
 * <pre>
 * IMAccount account("bob@hotmail.com", "bob", EnumIMProtocol::IMProtocolMSN);
 * IMConnect connect(account);
 * connect.connect();
 * //Waits that account is connected using IMConnect::LoginStatusConnected
 * IMChat * chat = IMWrapperFactory::getFactory().createIMChat(account);
 * //Waits for IMChat::newIMChatSessionCreatedEvent and gets the IMChatSession
 * //Waits for IMChatSession::messageReceivedEvent
 * <pre>
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class IMChatSession {
public:

	/**
	 * Emitted when a message has been received. The message is queued and must
	 * be taken with getReceivedMessage()
	 *
	 * @param sender this class
	 */
	Event<void (IMChatSession & sender)> messageReceivedEvent;

	/**
	 * Emitted when a status message has been received. (e.g "Joe is connected")
	 *
	 * @param sender this class
	 * @param status the type of status message
	 * @param message a message describing the status
	 */
	Event<void (IMChatSession & sender, IMChat::StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	/**
	 * Typing state event.
	 *
	 * @param sender this class
	 * @param contactId contact who typing state has changed
	 * @param state @see TypingState
	 */
	Event<void (IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state)> typingStateChangedEvent;

	/**
	 * Emitted when an IMContact has been added in this IMChatSession.
	 *
	 * @param sender this class
	 * @param imContact the added IMContact
	 */
	Event<void (IMChatSession & sender, const IMContact & imContact)> contactAddedEvent;

	/**
	 * Emitted when an IMContact has been removed.
	 *
	 * @param sender this class
	 * @param imContact the removed IMContact
	 */
	Event<void (IMChatSession & sender, const IMContact & imContact)> contactRemovedEvent;

	/**
	 * Emitted when this IMChatSession will be closed and destroyed.
	 *
	 * @param sender this class
	 */
	Event<void (IMChatSession & sender) > imChatSessionWillDieEvent;

	class IMChatMessage {
	public:

		IMChatMessage(const IMContact & imContact, const std::string & message);

		const IMContact & getIMContact() const { return _imContact; }

		std::string getMessage() const { return _message; }

	private:

		const IMContact & _imContact;

		std::string _message;

	};

	/**
	 * Gets the first message queued in IMChatSession. Delete it from the queue.
	 *
	 * @return the first message of the queue. NULL if no message in the queue.
	 * The returned IMChatMessage must be freed by the called.
	 */
	IMChatMessage * getReceivedMessage();

	/**
	 * Constructs a chat session given a IMChat.
	 *
	 * @param imChat because a chat session is associated to 1 IMAccount
	 */
	IMChatSession(IMChat & imChat);

	~IMChatSession();

	/**
	 * Close the IMChatSession.
	 *
	 * An imChatSessionWillDieEvent is sent then the IMChatSession object is
	 * destroyed.
	 */
	void close();

	/**
	 * Adds a contact to the chat session.
	 *
	 * @param imContact contact that will receive messages from this chat session
	 */
	void addIMContact(const IMContact & imContact);

	void removeIMContact(const IMContact & imContact);

	void removeAllIMContact();

	/**
	 * @return true if this IMChatSession can do multi chat.
	 * A session can do multi chat if it is on a particular protocol.
	 */
	bool canDoMultiChat();

	/**
	 * Sends a chat message to all the contacts from this chat session.
	 *
	 * @param message chat message to send
	 */
	void sendMessage(const std::string & message);

	/**
	 * Changes my typing state to inform all IMContact linked to the chat session.
	 *
	 * @param state my current typing state
	 */
	void changeTypingState(IMChat::TypingState state);


	const IMContactSet & getIMContactSet() const {
		return _imContactSet;
	}

	IMChat & getIMChat() { return _imChat; }

	bool operator==(const IMChatSession & imChatSession) const;

	int getId() const;

private:

	void messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, const std::string & message);

	void statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message);

	void typingStateChangedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, IMChat::TypingState state);

	void contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	void contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	IMContactSet _imContactSet;

	IMChat & _imChat;

	typedef std::list<IMChatMessage *> IMChatMessageList;

	IMChatMessageList _receivedIMChatMessageList;

};

#endif //IMCHATSESSION_H
