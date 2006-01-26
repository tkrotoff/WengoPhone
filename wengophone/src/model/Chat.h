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

#ifndef CHAT_H
#define CHAT_H

#include <model/imwrapper/IMChat.h>

#include <Interface.h>
#include <Event.h>

#include <string>

class IMAccount;

/**
 * Wrapper for Instant Messaging chat.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Chat : Interface {
public:

	/**
	 * @see IMChat::messageReceivedEvent
	 */
	Event<void (IMChat & sender, int session, const std::string & from, const std::string & message)> messageReceivedEvent;

	/**
	 * @see IMChat::statusMessageEvent
	 */
	Event<void (IMChat & sender, int session, IMChat::StatusMessage status, const std::string & message)> statusMessageEvent;

	Chat(IMAccount & account);

	virtual ~Chat() { }

	/**
	 * @see IMChat::createSession
	 */
	int createSession();

	/**
	 * @see IMChat::closeSession
	 */
	void closeSession(int session);

	/**
	 * @see IMChat::sendMessage
	 */
	void sendMessage(int session, const std::string & message);

	/**
	 * @see IMChat::addContact
	 */
	void addContact(int session, const std::string & contactId);

	/**
	 * @see IMChat::reomveContact
	 */
	void removeContact(int session, const std::string & contactId);

protected:

	IMAccount & _imAccount;
	
	IMChat * _imChat;
};

#endif	//CHAT_H
