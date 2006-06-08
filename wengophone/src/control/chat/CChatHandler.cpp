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

#include "CChatHandler.h"

#include <presentation/PChatHandler.h>
#include <presentation/PFactory.h>

#include <model/chat/ChatHandler.h>

#include <util/Logger.h>

CChatHandler::CChatHandler(ChatHandler & chatHandler, CUserProfile & cUserProfile)
	: _chatHandler(chatHandler), _cUserProfile(cUserProfile) {

	_pChatHandler = PFactory::getFactory().createPresentationChatHandler(*this);

	_chatHandler.newIMChatSessionCreatedEvent +=
		boost::bind(&CChatHandler::newIMChatSessionCreatedEventHandler, this, _1, _2);
}

CChatHandler::~CChatHandler() {
	_chatHandler.newIMChatSessionCreatedEvent -=
		boost::bind(&CChatHandler::newIMChatSessionCreatedEventHandler, this, _1, _2);

	delete _pChatHandler;
}

void CChatHandler::newIMChatSessionCreatedEventHandler(ChatHandler & sender, IMChatSession & imChatSession) {
	LOG_DEBUG("new IMChatSessionCreatedEvent");
	_pChatHandler->newIMChatSessionCreatedEventHandler(imChatSession);
}

void CChatHandler::createSession(IMAccount & imAccount, IMContactSet & imContactSet) {
	_chatHandler.createSession(imAccount, imContactSet);
}
