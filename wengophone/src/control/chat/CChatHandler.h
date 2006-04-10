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

#ifndef CCHATHANDLER_H
#define CCHATHANDLER_H

#include <imwrapper/IMChat.h>

#include <util/NonCopyable.h>
#include <util/Event.h>

class IMAccount;
class IMContactSet;
class ChatHandler;
class PChatHandler;
class UserProfile;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class CChatHandler : NonCopyable {
public:

	/**
	 * Emitted when a new IMChatSession has been created.
	 *
	 * @param sender this class
	 * @param imChatSession the new IMChatSession
	 */
	Event<void (ChatHandler & sender, IMChatSession & imChatSession)> newIMChatSessionCreatedEvent;

	void createSession(const IMAccount & imAccount, IMContactSet & imContactSet);

	UserProfile & getUserProfile() const { return _userProfile;}

	CChatHandler(ChatHandler & chatHandler, UserProfile & userProfile);

	~CChatHandler();

private:

	void newIMChatSessionCreatedEventHandler(ChatHandler & sender, IMChatSession & imChatSession);

	ChatHandler & _chatHandler;

	PChatHandler * _pChatHandler;

	UserProfile & _userProfile;

};

#endif	//CCHATHANDLER_H
