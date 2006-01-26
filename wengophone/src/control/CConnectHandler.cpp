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

#include "CConnectHandler.h"

#include <model/ConnectHandler.h>
#include <control/CChatHandler.h>
#include <control/presence/CPresenceHandler.h>
#include <presentation/PFactory.h>

CConnectHandler::CConnectHandler(ConnectHandler & connectHandler) 
	: _connectHandler(connectHandler) {

	_cPresenceHandler = NULL;
	_cChatHandler = NULL;

	_pConnectHandler = PFactory::getFactory().createPresentationConnectHandler(*this);

	_connectHandler.loginStatusEvent += loginStatusEvent;
	_connectHandler.presenceHandlerCreatedEvent += 
		boost::bind(&CConnectHandler::presenceHandlerCreatedEventHandler, this, _1, _2);
	_connectHandler.chatHandlerCreatedEvent += 
		boost::bind(&CConnectHandler::chatHandlerCreatedEventHandler, this, _1, _2);
}

CConnectHandler::~CConnectHandler() {
	if (_cPresenceHandler)
		delete _cPresenceHandler;

	if (_cChatHandler)
		delete _cChatHandler;
}

void CConnectHandler::presenceHandlerCreatedEventHandler(ConnectHandler & sender, PresenceHandler & presenceHandler) {
	_cPresenceHandler = new CPresenceHandler(presenceHandler);
}

void CConnectHandler::chatHandlerCreatedEventHandler(ConnectHandler & sender, ChatHandler & chatHandler) {
	_cChatHandler = new CChatHandler(chatHandler);
}
