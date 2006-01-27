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
#include <List.h>

#include <string>

class IMAccount;
class IMChatSession;

/**
 * Wrapper for Instant Messaging chat.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Chat : Interface {
public:

	Chat(IMAccount & account);

	virtual ~Chat() { }

	IMChatSession & createSession();

protected:

	void messageReceivedEventHandler(IMChat & sender, IMChatSession & chatSession, const std::string & from, const std::string & message);

	void statusMessageEventHandler(IMChat & sender, IMChatSession & chatSession, IMChat::StatusMessage status, const std::string & message);

	List<IMChatSession *> _imChatSessionList;

	IMAccount & _imAccount;

	IMChat * _imChat;

};

#endif	//CHAT_H
