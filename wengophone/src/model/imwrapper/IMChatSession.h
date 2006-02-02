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

#include "IMChat.h"

#include <string>
#include <set>

class IMAccount;
class IMAccountHandler;
class IMContact;
class IMChat;
class IMChatMap;
class ChatHandler;

class IMChatSession {
	friend class ChatHandler;
public:

	typedef std::set<const IMContact *> IMContactList;

	Event<void (IMChatSession & sender, const IMContact & from, const std::string & message)> messageReceivedEvent;

	Event<void (IMChatSession & sender, IMChat::StatusMessage status, const std::string & message)> statusMessageEvent;

	IMChatSession(IMChatMap & imChatMap);

	~IMChatSession();

	void addIMContact(const IMContact & imContact);

	void removeIMContact(const IMContact & imContact);

	void sendMessage(const std::string & message);

	const IMContactList & getIMContactList() const;

	bool operator == (const IMChatSession & imChatSession) const;

	int getId() const;

private:

	typedef std::set<IMChat *> IMChatSet;

	void messageReceivedEventHandler(IMChat & sender, IMChatSession * imChatSession, const std::string & from, const std::string & message);

	void statusMessageEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message);

	const IMContact * getIMContact(const IMAccount & imAccount, const std::string & contactId) const;

	/**
	 * Update the IMChatList regarding IMContactList.
	 */
	void updateIMChatList();

	IMContactList _imContactList;

	IMChatSet _imChatSet;

	IMChatMap & _imChatMap;

};

#endif //IMCHATSESSION_H
