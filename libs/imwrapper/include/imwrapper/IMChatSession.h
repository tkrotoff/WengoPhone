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

#include <string>
#include <set>

class IMAccount;
class IMAccountHandler;
class IMContact;
class IMChat;
class ChatHandler;

class IMChatSession {
	friend class ChatHandler;
public:

	/** List of IMContact, each IMContact is unique inside the list, thus we use a set. */
	typedef std::set<IMContact> IMContactList;

	Event<void (IMChatSession & sender, const IMContact & from, const std::string & message)> messageReceivedEvent;

	Event<void (IMChatSession & sender, IMChat::StatusMessage status, const std::string & message)> statusMessageReceivedEvent;

	Event<void (IMChatSession & sender, const IMContact & imContact)> contactAddedEvent;

	Event<void (IMChatSession & sender, const IMContact & imContact)> contactRemovedEvent;

	IMChatSession(IMChat & imChat);

	~IMChatSession();

	void addIMContact(const IMContact & imContact);

	void removeIMContact(const IMContact & imContact);

	void sendMessage(const std::string & message);

	const IMContactList & getIMContactList() const {
		return _imContactList;
	}

	bool operator==(const IMChatSession & imChatSession) const;

	int getId() const;

private:

	void messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, const std::string & message);

	void statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message);

	void contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	void contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId);

	IMContactList _imContactList;

	IMChat & _imChat;
};

#endif //IMCHATSESSION_H
