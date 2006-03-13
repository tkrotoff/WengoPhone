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

	Event<void (IMChatSession & sender, const IMContact & from, const std::string & message)> messageReceivedEvent;

	Event<void (IMChatSession & sender, IMChat::StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	Event<void (IMChatSession & sender, const IMContact & imContact)> contactAddedEvent;

	Event<void (IMChatSession & sender, const IMContact & imContact)> contactRemovedEvent;

	/**
	 * Constructs a chat session given a IMChat.
	 *
	 * @param imChat because a chat session is associated to 1 IMAccount
	 */
	IMChatSession(IMChat & imChat);

	~IMChatSession();

	/**
	 * Adds a contact to the chat session.
	 *
	 * @param imContact contact that will receive messages from this chat session
	 */
	void addIMContact(const IMContact & imContact);

	void removeIMContact(const IMContact & imContact);

	void removeAllIMContact();

	/**
	 * Sends a chat message to all the contacts from this chat session.
	 *
	 * @param message chat message to send
	 */
	void sendMessage(const std::string & message);

	const IMContactSet & getIMContactSet() const {
		return _imContactSet;
	}

	IMChat & getIMChat() { return _imChat; }

	bool operator==(const IMChatSession & imChatSession) const;

	int getId() const;

private:

	void messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, const std::string & message);

	void statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message);

	void contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	void contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	IMContactSet _imContactSet;

	IMChat & _imChat;
};

#endif //IMCHATSESSION_H
