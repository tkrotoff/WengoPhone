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

#include <model/connect/ConnectHandler.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMWrapperFactory.h>

#include <util/Logger.h>

using namespace std;

ChatHandler::ChatHandler(UserProfile & userProfile) {
	userProfile.newIMAccountAddedEvent +=
		boost::bind(&ChatHandler::newIMAccountAddedEventHandler, this, _1, _2);
	userProfile.getConnectHandler().connectedEvent +=
		boost::bind(&ChatHandler::connectedEventHandler, this, _1, _2);
	userProfile.getConnectHandler().disconnectedEvent +=
		boost::bind(&ChatHandler::disconnectedEventHandler, this, _1, _2);
}

ChatHandler::~ChatHandler() {
	for (IMChatSessionSet::iterator it = _imChatSessionSet.begin(); it != _imChatSessionSet.end(); it++) {
		delete (*it);
	}
}

void ChatHandler::createSession(IMAccount & imAccount, IMContactSet & imContactSet) {
	LOG_DEBUG("creating new IMChatSession for: " + imAccount.getLogin());
	_imChatMap[&imAccount]->createSession(imContactSet);
}

void ChatHandler::connectedEventHandler(ConnectHandler & sender, IMAccount & account) {
	LOG_DEBUG("an account is connected: login: " + account.getLogin()
		+ " protocol: " + String::fromNumber(account.getProtocol()));
}

void ChatHandler::disconnectedEventHandler(ConnectHandler & sender, IMAccount & account) {
}

void ChatHandler::newIMChatSessionCreatedEventHandler(IMChat & sender, IMChatSession & imChatSession) {
	LOG_DEBUG("a new IMChatSession has been created");
	_imChatSessionSet.insert(&imChatSession);
	imChatSession.imChatSessionWillDieEvent += 
		boost::bind(&ChatHandler::imChatSessionWillDieEventHandler, this, _1);
	newIMChatSessionCreatedEvent(*this, imChatSession);
}

void ChatHandler::imChatSessionWillDieEventHandler(IMChatSession & sender) {
	IMChatSessionSet::iterator it = _imChatSessionSet.find(&sender);

	if (it != _imChatSessionSet.end()) {
		_imChatSessionSet.erase(it);
	} else {
		LOG_ERROR("IMChatSession not in ChatHandler");
	}
}

void ChatHandler::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	IMChatMap::const_iterator it = _imChatMap.find(&imAccount);

	LOG_DEBUG("new account added: login: " + imAccount.getLogin()
		+ " protocol: " + String::fromNumber(imAccount.getProtocol()));
	//IMChat for this IMAccount has not been created yet
	if (it == _imChatMap.end()) {
		IMChat * imChat = IMWrapperFactory::getFactory().createIMChat(imAccount);
		imChat->newIMChatSessionCreatedEvent +=
			boost::bind(&ChatHandler::newIMChatSessionCreatedEventHandler, this, _1, _2);

		_imChatMap[&imAccount] = imChat;

		imAccount.imAccountDeadEvent +=
			boost::bind(&ChatHandler::imAccountDeadEventHandler, this, _1);

	} else {
		LOG_ERROR("this IMAccount has already been added " + imAccount.getLogin());
	}
}

void ChatHandler::imAccountDeadEventHandler(IMAccount & sender) {
	IMChatMap::iterator it = _imChatMap.find(&sender);

	if (it != _imChatMap.end()) {
		//TODO: close all IMChatSession opened with this IMAccount
		delete (*it).second;
		_imChatMap.erase(it);
	} else {
		LOG_ERROR("this IMAccount has not been added " + sender.getLogin());
	}
}
