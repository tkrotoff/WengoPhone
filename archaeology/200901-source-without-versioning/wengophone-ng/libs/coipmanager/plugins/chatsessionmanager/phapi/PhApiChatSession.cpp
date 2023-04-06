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

#include "PhApiChatSession.h"

#include "PhApiChatSessionManager.h"

#include <coipmanager_base/account/SipAccount.h>
#include <coipmanager_base/imcontact/SipIMContact.h>

#include <util/Logger.h>

#include <sipwrapper/SipChatWrapper.h>
#include <sipwrapper/SipWrapper.h>
#include <PhApiWrapper.h>

PhApiChatSession::PhApiChatSession(CoIpManager & coIpManager,
	PhApiChatSessionManager & phApiChatSessionManager)
	: IChatSessionPlugin(coIpManager),
	_phApiChatSessionManager(phApiChatSessionManager) {

	_sipAccount = NULL;
	_sipWrapper = PhApiWrapper::getInstance();
	
	_phApiChatSessionManager.registerChatSession(this);
}

PhApiChatSession::~PhApiChatSession() {
	_phApiChatSessionManager.unregisterChatSession(this);
}

void PhApiChatSession::sendMessage(const std::string & message) {
	if (!_sipAccount) {
		LOG_FATAL("session not started");
		return;
	}

	for (IMContactList::const_iterator it = _imContactList.begin(); it != _imContactList.end(); it++) {
		SipIMContact * sipIMContact = static_cast<SipIMContact *>((*it).getPrivateIMContact());
		LOG_DEBUG("toString: " + sipIMContact->toString());
		_sipWrapper->getSipChatWrapper()->sendMessage(_sipAccount->getVirtualLineId(),
			sipIMContact->getFullIdentity().c_str(),
			message.c_str());
	}
}

void PhApiChatSession::setTypingState(EnumChatTypingState::ChatTypingState state) {
	if (!_sipAccount) {
		LOG_FATAL("session not started");
		return;
	}

	for (IMContactList::const_iterator it = _imContactList.begin(); it != _imContactList.end(); it++) {
		SipIMContact * sipIMContact = static_cast<SipIMContact *>((*it).getPrivateIMContact());
		_sipWrapper->getSipChatWrapper()->sendTypingState(_sipAccount->getVirtualLineId(),
			sipIMContact->getFullIdentity().c_str(),
			state);
	}
}

void PhApiChatSession::start() {
	_sipAccount = (SipAccount *) _account.getPrivateAccount();
}

void PhApiChatSession::pause() {
}

void PhApiChatSession::resume() {
}

void PhApiChatSession::stop() {
	_sipAccount = NULL;
}
