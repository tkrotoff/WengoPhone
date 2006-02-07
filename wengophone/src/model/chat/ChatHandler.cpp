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

#include "ChatHandler.h"

#include <model/WengoPhone.h>
#include <model/connect/ConnectHandler.h>

#include <IMAccount.h>
#include <IMChatSession.h>
#include <IMWrapperFactory.h>
#include <Logger.h>

using namespace std;

ChatHandler::ChatHandler(WengoPhone & wengoPhone) 
	: _wengoPhone(wengoPhone) {
	_wengoPhone.getConnectHandler().connectedEvent += 
		boost::bind(&ChatHandler::connectedEventHandler, this, _1, _2);
	_wengoPhone.getConnectHandler().disconnectedEvent += 
		boost::bind(&ChatHandler::disconnectedEventHandler, this, _1, _2);	
}

ChatHandler::~ChatHandler() {
	for (IMChatMap::iterator i = _imChatMap.begin() ; i != _imChatMap.end() ; i++) {
		delete (*i).second;
	}

	for (IMChatSessionList::iterator i = _imChatSessionList.begin() ; i != _imChatSessionList.end() ; i++) {
		delete (*i);
	}
}

void ChatHandler::createSession(const IMAccount & imAccount) {
	LOG_DEBUG("creating new IMChatSession for: " + imAccount.getLogin());
	_imChatMap[&((IMAccount &)imAccount)]->createSession();
}

void ChatHandler::connectedEventHandler(ConnectHandler & sender, IMAccount & account) {
	IMChatMap::iterator i = _imChatMap.find(&account);
	
	LOG_DEBUG("an account is connected: login: " + account.getLogin() 
		+ "protocol: " + String::fromNumber(account.getProtocol()));
	//IMChat for this IMAccount has not been created yet
	if (i == _imChatMap.end()) {
		IMChat * imChat = IMWrapperFactory::getFactory().createIMChat(account, _wengoPhone.getIMContactMap());
		imChat->newIMChatSessionCreatedEvent +=
			boost::bind(&ChatHandler::newIMChatSessionCreatedEventHandler, this, _1, _2);

		_imChatMap[&account] = imChat;
	}
}

void ChatHandler::disconnectedEventHandler(ConnectHandler & sender, IMAccount & account) {
}

void ChatHandler::newIMChatSessionCreatedEventHandler(IMChat & sender, IMChatSession & imChatSession) {
	LOG_DEBUG("a new IMChatSession has been created");
	_imChatSessionList.push_back(&imChatSession);
	newIMChatSessionCreatedEvent(*this, imChatSession);
}
