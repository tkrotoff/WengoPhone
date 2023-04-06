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

#include "IAXClientCallSessionManager.h"

#include "IAXClientCallSession.h"

#include <IAXClientFactory.h>
#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

IAXClientCallSessionManager::IAXClientCallSessionManager(CoIpManager & coIpManager)
	: ICallSessionManagerPlugin(coIpManager) {

	_supportedAccountType.push_back(EnumAccountType::AccountTypeIAX);

	//Get IAXClientWrapper instance
	IAXClientFactory iaxFactory;
	_iaxClientWrapper = iaxFactory.createSipWrapper();

	//Bind to get incoming call event
	_iaxClientWrapper->phoneCallStateChangedEvent.connect(this,
		boost::bind(&IAXClientCallSessionManager::phoneCallStateChangedEventHandler, this, _1, _2, _3, _4), NULL);
}

IAXClientCallSessionManager::~IAXClientCallSessionManager() {
}

ICallSessionPlugin * IAXClientCallSessionManager::createICallSessionPlugin() {
	return new IAXClientCallSession(_coIpManager, _iaxClientWrapper);
}

void IAXClientCallSessionManager::phoneCallStateChangedEventHandler(SipWrapper * sender, int callId,
	EnumPhoneCallState::PhoneCallState state, const std::string & from) {

	if (sender != _iaxClientWrapper) {
		LOG_ERROR("phoneCallStateChangedEvent not coming from our wrapper");
		return;
	}

	if (state != EnumPhoneCallState::PhoneCallStateIncoming) {
		LOG_ERROR("this is not an incoming phone call");
		return;
	}

	LOG_DEBUG("incoming phone call, create a CallSession");

	IAXClientCallSession * session = new IAXClientCallSession(_coIpManager, _iaxClientWrapper, callId, state, from);
	newICallSessionPluginCreatedEvent(this, session);
}
