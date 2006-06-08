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

#include "CWengoPhone.h"

#include <presentation/PFactory.h>
#include <presentation/PWengoPhone.h>

#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/webservices/subscribe/CSubscribe.h>

#include <model/WengoPhone.h>
#include <model/webservices/subscribe/WsSubscribe.h>

#include <util/Logger.h>

using namespace std;

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone),
	_cUserProfileHandler(wengoPhone.getUserProfileHandler(), _wengoPhone)  {

	_cUserProfile = NULL;

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_wengoPhone.initFinishedEvent +=
		boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.timeoutEvent += controlTimeoutEvent;
	_wengoPhone.wsSubscribeCreatedEvent +=
		boost::bind(&CWengoPhone::wsSubscribeCreatedEventHandler, this, _1, _2);

	_wengoPhone.getUserProfileHandler().noCurrentUserProfileSetEvent +=
		boost::bind(&CWengoPhone::noCurrentUserProfileSetEventHandler, this, _1);
	_wengoPhone.getUserProfileHandler().currentUserProfileWillDieEvent +=
		boost::bind(&CWengoPhone::currentUserProfileWillDieEventHandler, this, _1);
	_wengoPhone.getUserProfileHandler().userProfileInitializedEvent +=
		boost::bind(&CWengoPhone::userProfileInitializedEventHandler, this, _1, _2);
	_wengoPhone.getUserProfileHandler().wengoAccountNotValidEvent +=
		boost::bind(&CWengoPhone::wengoAccountNotValidEventHandler, this, _1, _2);
}

void CWengoPhone::start() {
	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::wsSubscribeCreatedEventHandler(WengoPhone & sender, WsSubscribe & wsSubscribe) {
	static CSubscribe cSubscribe(wsSubscribe, *this);

	LOG_DEBUG("CSubscribe created");
}

void CWengoPhone::authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
	const std::string & message) {
	_pWengoPhone->authorizationRequestEventHandler(sender, imContact, message);
}

void CWengoPhone::noCurrentUserProfileSetEventHandler(UserProfileHandler & sender) {
	_pWengoPhone->noCurrentUserProfileSetEventHandler();
}

void CWengoPhone::currentUserProfileWillDieEventHandler(UserProfileHandler & sender) {
	_pWengoPhone->currentUserProfileWillDieEventHandler();
}

void CWengoPhone::currentUserProfileReleased() {
	_cUserProfile->getUserProfile().getPresenceHandler().authorizationRequestEvent -=
		boost::bind(&CWengoPhone::authorizationRequestEventHandler, this, _1, _2, _3);

	if (_cUserProfile) {
		delete _cUserProfile;
		_cUserProfile = NULL;
	}

	_cUserProfileHandler.currentUserProfileReleased();
}

void CWengoPhone::userProfileInitializedEventHandler(UserProfileHandler & sender, UserProfile & userProfile) {
	_cUserProfile = new CUserProfile(userProfile, *this, _wengoPhone);

	userProfile.loginStateChangedEvent += loginStateChangedEvent;
	userProfile.networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	userProfile.proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	userProfile.wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	userProfile.getPresenceHandler().authorizationRequestEvent +=
		boost::bind(&CWengoPhone::authorizationRequestEventHandler, this, _1, _2, _3);
	
	_pWengoPhone->userProfileInitializedEventHandler();
}

void CWengoPhone::wengoAccountNotValidEventHandler(UserProfileHandler & sender, WengoAccount & wengoAccount) {
	_pWengoPhone->wengoAccountNotValidEventHandler(wengoAccount);
}
