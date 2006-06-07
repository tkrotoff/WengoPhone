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

#include <model/WengoPhone.h>
#include <model/wenbox/WenboxPlugin.h>
#include <model/contactlist/Contact.h>
#include <model/connect/ConnectHandler.h>
#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/history/History.h>
#include <model/webservices/sms/WsSms.h>
#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <model/webservices/subscribe/WsSubscribe.h>
#include <model/webservices/directory/WsDirectory.h>

#include <presentation/PFactory.h>
#include <presentation/PWengoPhone.h>

#include <control/phoneline/CPhoneLine.h>
#include <control/contactlist/CContactList.h>
#include <control/history/CHistory.h>
#include <control/wenbox/CWenboxPlugin.h>
#include <control/chat/CChatHandler.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/webservices/sms/CSms.h>
#include <control/webservices/softupdate/CSoftUpdate.h>
#include <control/webservices/subscribe/CSubscribe.h>
#include <control/webservices/directory/CWsDirectory.h>
#include <control/webservices/callforward/CWsCallForward.h>

#include <util/Logger.h>

using namespace std;

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone),
	_cUserProfileHandler(wengoPhone.getUserProfileHandler(), _wengoPhone)  {

	_cUserProfile = NULL;
	_cWenboxPlugin = NULL;
	_cChatHandler = NULL;
	_cWsDirectory = NULL;
	_cHistory = NULL;
	_cSms = NULL;
	_cSoftUpdate = NULL;
	_cPhoneLine = NULL;

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

	_cWsCallForward = NULL;
}

void CWengoPhone::start() {
	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	_cPhoneLine = new CPhoneLine(phoneLine, *this);

	LOG_DEBUG("CPhoneLine created");
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::wsSubscribeCreatedEventHandler(WengoPhone & sender, WsSubscribe & wsSubscribe) {
	static CSubscribe cSubscribe(wsSubscribe, *this);

	LOG_DEBUG("CWenboxPlugin created");
}

void CWengoPhone::wsDirectoryCreatedEventHandler(UserProfile & sender, WsDirectory & wsDirectory) {
	if (!_cWsDirectory) {
		_cWsDirectory = new CWsDirectory(*this, wsDirectory);
		LOG_DEBUG("CWsDirectory created");
	}
}

void CWengoPhone::wsSmsCreatedEventHandler(UserProfile & sender, WsSms & sms) {
	if (!_cSms) {
		_cSms = new CSms(sms, *this);
		LOG_DEBUG("CSms created");
	}
}

void CWengoPhone::wsCallForwardCreatedEventHandler(UserProfile & sender, WsCallForward & wsCallForward) {
	_cWsCallForward = new CWsCallForward(*this, wsCallForward);
}

void CWengoPhone::wsSoftUpdateCreatedEventHandler(UserProfile & sender, WsSoftUpdate & wsSoftUpdate) {
	if (!_cSoftUpdate) {
		_cSoftUpdate = new CSoftUpdate(wsSoftUpdate, *this);
		LOG_DEBUG("CSoftUpdate created");
	}
}

PhoneCall * CWengoPhone::getActivePhoneCall() const {
	if (_cUserProfile) {
		//FIXME: model must not be used directly by the GUI
		IPhoneLine * phoneLine = _cUserProfile->getUserProfile().getActivePhoneLine();
		if (phoneLine) {
			return phoneLine->getActivePhoneCall();
		}
		return NULL;
	}
}


void CWengoPhone::historyLoadedEventHandler(History & history) {
	_cHistory = new CHistory(history, *this, _wengoPhone);
	cHistoryCreatedEvent(*this, *_cHistory);
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
	_cUserProfile->getUserProfile().wsDirectoryCreatedEvent -=
		boost::bind(&CWengoPhone::wsDirectoryCreatedEventHandler, this, _1, _2);
	_cUserProfile->getUserProfile().phoneLineCreatedEvent -=
		boost::bind(&CWengoPhone::phoneLineCreatedEventHandler, this, _1, _2);
	_cUserProfile->getUserProfile().wsSmsCreatedEvent -=
		boost::bind(&CWengoPhone::wsSmsCreatedEventHandler, this, _1, _2);
	_cUserProfile->getUserProfile().wsSoftUpdateCreatedEvent -=
		boost::bind(&CWengoPhone::wsSoftUpdateCreatedEventHandler, this, _1, _2);
	_cUserProfile->getUserProfile().getHistory().historyLoadedEvent -=
		boost::bind(&CWengoPhone::historyLoadedEventHandler, this, _1);
	_cUserProfile->getUserProfile().wsCallForwardCreatedEvent -=
	  boost::bind(&CWengoPhone::wsCallForwardCreatedEventHandler, this, _1, _2);
	_cUserProfile->getUserProfile().getPresenceHandler().authorizationRequestEvent -=
		boost::bind(&CWengoPhone::authorizationRequestEventHandler, this, _1, _2, _3);


	if (_cWenboxPlugin) {
		delete _cWenboxPlugin;
		_cWenboxPlugin = NULL;
	}

	if (_cWsDirectory) {
		delete _cWsDirectory;
		_cWsDirectory = NULL;
	}

	if (_cHistory) {
		delete _cHistory;
		_cHistory = NULL;
	}

	if (_cChatHandler) {
		delete _cChatHandler;
		_cChatHandler = NULL;
	}

	if (_cSms) {
		delete _cSms;
		_cSms = NULL;
	}

	if (_cSoftUpdate) {
		delete _cSoftUpdate;
		_cSoftUpdate = NULL;
	}

	if (_cPhoneLine) {
		delete _cPhoneLine;
		_cPhoneLine = NULL;
	}

	if (_cUserProfile) {
		delete _cUserProfile;
		_cUserProfile = NULL;
	}

	_cUserProfileHandler.currentUserProfileReleased();
}

void CWengoPhone::userProfileInitializedEventHandler(UserProfileHandler & sender, UserProfile & userProfile) {
	_cUserProfile = new CUserProfile(userProfile, _wengoPhone);
	_cWenboxPlugin = new CWenboxPlugin(*userProfile.getWenboxPlugin(), *this);
	_cChatHandler = new CChatHandler(userProfile.getChatHandler(), *this, userProfile);

	userProfile.loginStateChangedEvent += loginStateChangedEvent;
	userProfile.networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	userProfile.proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	userProfile.wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	userProfile.wsDirectoryCreatedEvent +=
		boost::bind(&CWengoPhone::wsDirectoryCreatedEventHandler, this, _1, _2);
	userProfile.phoneLineCreatedEvent +=
		boost::bind(&CWengoPhone::phoneLineCreatedEventHandler, this, _1, _2);
	userProfile.wsSmsCreatedEvent +=
		boost::bind(&CWengoPhone::wsSmsCreatedEventHandler, this, _1, _2);
	userProfile.wsSoftUpdateCreatedEvent +=
		boost::bind(&CWengoPhone::wsSoftUpdateCreatedEventHandler, this, _1, _2);
	userProfile.getHistory().historyLoadedEvent +=
		boost::bind(&CWengoPhone::historyLoadedEventHandler, this, _1);
	userProfile.wsCallForwardCreatedEvent +=
	  boost::bind(&CWengoPhone::wsCallForwardCreatedEventHandler, this, _1, _2);
	userProfile.getPresenceHandler().authorizationRequestEvent +=
		boost::bind(&CWengoPhone::authorizationRequestEventHandler, this, _1, _2, _3);
	
	_pWengoPhone->userProfileInitializedEventHandler();
}

void CWengoPhone::wengoAccountNotValidEventHandler(UserProfileHandler & sender, WengoAccount & wengoAccount) {
	_pWengoPhone->wengoAccountNotValidEventHandler(wengoAccount);
}
