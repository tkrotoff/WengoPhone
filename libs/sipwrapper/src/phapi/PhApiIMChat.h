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

#ifndef PHAPIIMCHAT_H
#define PHAPIIMCHAT_H

#include "PhApiFactory.h"

#include <IMChat.h>

class IMAccount;
class IMChatSession;

/**
 * PhApi IM chat.
 *
 * @ingroup model
 * @author Mathieu Stute
 */
class PhApiIMChat : public IMChat {
	friend class PhApiFactory;
public:

	void sendMessage(IMChatSession & chatSession, const std::string & message);
	void createSession();
	void closeSession(IMChatSession & chatSession);
	void addContact(IMChatSession & chatSession, const std::string & contactId);
	void removeContact(IMChatSession & chatSession, const std::string & contactId);

	static PhApiIMChat * PhApiIMChatHack;

private:

	PhApiIMChat(IMAccount & account, PhApiWrapper & phApiWrapper);

	void messageReceivedEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, const std::string & from, const std::string & message);

	void statusMessageReceivedEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, StatusMessage status, const std::string & message);

	void newIMChatSessionCreatedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession);

	void contactAddedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession, const std::string & contactId);

	void contactRemovedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession, const std::string & contactId);

	PhApiWrapper & _phApiWrapper;
};

#endif	//PHAPIIMCHAT_H
