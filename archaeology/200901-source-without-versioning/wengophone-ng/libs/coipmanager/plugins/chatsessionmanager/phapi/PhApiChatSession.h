/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWPHAPICHATSESSION_H
#define OWPHAPICHATSESSION_H

#include "PhApiChatSessionManager.h"

#include <coipmanager/chatsessionmanager/IChatSessionPlugin.h>

class SipAccount;
class SipWrapper;

/**
 * IChatSession PhApi implementation.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class PhApiChatSession : public IChatSessionPlugin {
	//PhApiChatSessionManager is friend with PhApiChatSession
	//so that PhApiChatSessionManager can emit signals from ChatSession
	friend class PhApiChatSessionManager;
public:

	PhApiChatSession(CoIpManager & coIpManager,
		PhApiChatSessionManager & phApiChatSessionManager);

	virtual ~PhApiChatSession();

	virtual void sendMessage(const std::string & message);

	virtual void setTypingState(EnumChatTypingState::ChatTypingState state);

	virtual void start();

	virtual void pause();

	virtual void resume();

	virtual void stop();

private:

	PhApiChatSessionManager & _phApiChatSessionManager;

	SipAccount *_sipAccount;

	SipWrapper *_sipWrapper;

};

#endif	//OWPHAPICHATSESSION_H
