/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "CUserProfile.h"

#include <presentation/PFactory.h>
#include <presentation/PUserProfile.h>

#include <control/CWengoPhone.h>
#include <control/chat/CChatHandler.h>
#include <control/contactlist/CContactList.h>
#include <control/history/CHistory.h>
#include <control/phoneline/CPhoneLine.h>
#include <control/webservices/sms/CSms.h>
#include <control/webservices/softupdate/CSoftUpdate.h>
#include <control/webservices/directory/CWsDirectory.h>
#include <control/webservices/callforward/CWsCallForward.h>
#include <control/wenbox/CWenboxPlugin.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/Contact.h>
#include <model/history/History.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/UserProfile.h>
#include <model/webservices/directory/WsDirectory.h>
#include <model/webservices/sms/WsSms.h>
#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <model/wenbox/WenboxPlugin.h>

#include <util/Logger.h>

CUserProfile::CUserProfile(UserProfile & userProfile, CWengoPhone & cWengoPhone, 
	Thread & modelThread) 
: _userProfile(userProfile),
_cWengoPhone(cWengoPhone),
_cContactList(userProfile.getContactList(), modelThread),
_cWenboxPlugin(*userProfile.getWenboxPlugin(), cWengoPhone),
_cChatHandler(userProfile.getChatHandler(), *this),
_modelThread(modelThread) {

	_cHistory = NULL;
	_cPhoneLine = NULL;
	_cSms = NULL;
	_cSoftUpdate = NULL;
	_cWsCallForward = NULL;
	_cWsDirectory = NULL;

	_pUserProfile = PFactory::getFactory().createPresentationUserProfile(*this);

	_userProfile.wsDirectoryCreatedEvent +=
		boost::bind(&CUserProfile::wsDirectoryCreatedEventHandler, this, _1, _2);
	_userProfile.phoneLineCreatedEvent +=
		boost::bind(&CUserProfile::phoneLineCreatedEventHandler, this, _1, _2);
	_userProfile.wsSmsCreatedEvent +=
		boost::bind(&CUserProfile::wsSmsCreatedEventHandler, this, _1, _2);
	_userProfile.wsSoftUpdateCreatedEvent +=
		boost::bind(&CUserProfile::wsSoftUpdateCreatedEventHandler, this, _1, _2);
	_userProfile.wsCallForwardCreatedEvent +=
	  boost::bind(&CUserProfile::wsCallForwardCreatedEventHandler, this, _1, _2);

	_userProfile.loginStateChangedEvent +=
		boost::bind(&CUserProfile::loginStateChangedEventHandler, this, _1, _2);
	_userProfile.networkDiscoveryStateChangedEvent +=
		boost::bind(&CUserProfile::networkDiscoveryStateChangedEventHandler, this, _1, _2);
	_userProfile.proxyNeedsAuthenticationEvent +=
		boost::bind(&CUserProfile::proxyNeedsAuthenticationEventHandler, this, _1, _2, _3);
	_userProfile.wrongProxyAuthenticationEvent +=
		boost::bind(&CUserProfile::wrongProxyAuthenticationEventHandler, this, _1, _2, _3, _4, _5);

	_userProfile.getHistory().historyLoadedEvent +=
		boost::bind(&CUserProfile::historyLoadedEventHandler, this, _1);

	_userProfile.getPresenceHandler().authorizationRequestEvent +=
		boost::bind(&CUserProfile::authorizationRequestEventHandler, this, _1, _2, _3);
}

CUserProfile::~CUserProfile() {
	_userProfile.wsDirectoryCreatedEvent -=
		boost::bind(&CUserProfile::wsDirectoryCreatedEventHandler, this, _1, _2);
	_userProfile.phoneLineCreatedEvent -=
		boost::bind(&CUserProfile::phoneLineCreatedEventHandler, this, _1, _2);
	_userProfile.wsSmsCreatedEvent -=
		boost::bind(&CUserProfile::wsSmsCreatedEventHandler, this, _1, _2);
	_userProfile.wsSoftUpdateCreatedEvent -=
		boost::bind(&CUserProfile::wsSoftUpdateCreatedEventHandler, this, _1, _2);
	_userProfile.getHistory().historyLoadedEvent -=
		boost::bind(&CUserProfile::historyLoadedEventHandler, this, _1);
	_userProfile.wsCallForwardCreatedEvent -=
	  boost::bind(&CUserProfile::wsCallForwardCreatedEventHandler, this, _1, _2);

	_userProfile.loginStateChangedEvent -=
		boost::bind(&CUserProfile::loginStateChangedEventHandler, this, _1, _2);
	_userProfile.networkDiscoveryStateChangedEvent -=
		boost::bind(&CUserProfile::networkDiscoveryStateChangedEventHandler, this, _1, _2);
	_userProfile.proxyNeedsAuthenticationEvent -=
		boost::bind(&CUserProfile::proxyNeedsAuthenticationEventHandler, this, _1, _2, _3);
	_userProfile.wrongProxyAuthenticationEvent -=
		boost::bind(&CUserProfile::wrongProxyAuthenticationEventHandler, this, _1, _2, _3, _4, _5);

	_userProfile.getHistory().historyLoadedEvent -=
		boost::bind(&CUserProfile::historyLoadedEventHandler, this, _1);

	_userProfile.getPresenceHandler().authorizationRequestEvent -=
		boost::bind(&CUserProfile::authorizationRequestEventHandler, this, _1, _2, _3);

	if (_pUserProfile) {
		delete _pUserProfile;
		_pUserProfile = NULL;
	}

	if (_cWsDirectory) {
		delete _cWsDirectory;
		_cWsDirectory = NULL;
	}

	if (_cHistory) {
		delete _cHistory;
		_cHistory = NULL;
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
}

void CUserProfile::loginStateChangedEventHandler(SipAccount & sender,
	SipAccount::LoginState state) {
	_pUserProfile->loginStateChangedEventHandler(sender, state);
}

void CUserProfile::networkDiscoveryStateChangedEventHandler(SipAccount & sender,
	SipAccount::NetworkDiscoveryState state) {
	_pUserProfile->networkDiscoveryStateChangedEventHandler(sender, state);
}

void CUserProfile::proxyNeedsAuthenticationEventHandler(SipAccount & sender,
	const std::string & proxyAddress, unsigned proxyPort) {
	_pUserProfile->proxyNeedsAuthenticationEventHandler(sender,
		proxyAddress, proxyPort);
}

void CUserProfile::wrongProxyAuthenticationEventHandler(SipAccount & sender,
	const std::string & proxyAddress, unsigned proxyPort,
	const std::string & proxyLogin, const std::string & proxyPassword) {
	_pUserProfile->wrongProxyAuthenticationEventHandler(sender, proxyAddress,
		proxyPort, proxyLogin, proxyPassword);
}

void CUserProfile::authorizationRequestEventHandler(PresenceHandler & sender,
	const IMContact & imContact, const std::string & message) {
	_pUserProfile->authorizationRequestEventHandler(sender, imContact, message);
}

void CUserProfile::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	_cPhoneLine = new CPhoneLine(phoneLine, _cWengoPhone);

	LOG_DEBUG("CPhoneLine created");
}

void CUserProfile::wsDirectoryCreatedEventHandler(UserProfile & sender, WsDirectory & wsDirectory) {
	if (!_cWsDirectory) {
		_cWsDirectory = new CWsDirectory(_cWengoPhone, wsDirectory);
		LOG_DEBUG("CWsDirectory created");
	}
}

void CUserProfile::wsSmsCreatedEventHandler(UserProfile & sender, WsSms & sms) {
	if (!_cSms) {
		_cSms = new CSms(sms, _cWengoPhone);
		LOG_DEBUG("CSms created");
	}
}

void CUserProfile::wsCallForwardCreatedEventHandler(UserProfile & sender, WsCallForward & wsCallForward) {
	_cWsCallForward = new CWsCallForward(_cWengoPhone, wsCallForward);
}

void CUserProfile::wsSoftUpdateCreatedEventHandler(UserProfile & sender, WsSoftUpdate & wsSoftUpdate) {
	if (!_cSoftUpdate) {
		_cSoftUpdate = new CSoftUpdate(wsSoftUpdate, _cWengoPhone);
		LOG_DEBUG("CSoftUpdate created");
	}
}

void CUserProfile::historyLoadedEventHandler(History & history) {
	_cHistory = new CHistory(history, _cWengoPhone, _cWengoPhone.getModelThread());
	_pUserProfile->cHistoryCreatedEventHandler();
}

void CUserProfile::disconnect() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::disconnectThreadSafe, this));

	_modelThread.postEvent(event);
}

void CUserProfile::disconnectThreadSafe() {
	_userProfile.disconnect();
}

void CUserProfile::makeContactCall(const std::string & contactId) {
	typedef ThreadEvent1<void (std::string contactId), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeContactCallThreadSafe, this, _1), contactId);

	_modelThread.postEvent(event);
}

void CUserProfile::makeContactCallThreadSafe(std::string contactId) {
	Contact * contact = _cContactList.getContact(contactId);
	if (contact) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		_userProfile.makeCall(*contact, config.getVideoEnable());
	}
}

void CUserProfile::makeCall(const std::string & phoneNumber) {
	typedef ThreadEvent1<void (std::string phoneNumber), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeCallThreadSafe, this, _1), phoneNumber);

	_modelThread.postEvent(event);
}

void CUserProfile::makeCallThreadSafe(std::string phoneNumber) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_userProfile.makeCall(phoneNumber, config.getVideoEnable());
}

void CUserProfile::makeCall(const std::string & phoneNumber, bool enableVideo) {
	typedef ThreadEvent2<void (std::string phoneNumber, bool enableVideo), std::string, bool> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeCallThreadSafe, this, _1, _2), phoneNumber, enableVideo);

	_modelThread.postEvent(event);
}

void CUserProfile::makeCallThreadSafe(std::string phoneNumber, bool enableVideo) {
	_userProfile.makeCall(phoneNumber, enableVideo);
}

void CUserProfile::startIM(const std::string & contactId) {
	typedef ThreadEvent1<void (std::string contactId), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::startIMThreadSafe, this, _1), contactId);

	_modelThread.postEvent(event);
}

void CUserProfile::startIMThreadSafe(std::string contactId) {
	Contact * contact = _cContactList.getContact(contactId);
	if (contact) {
		_userProfile.startIM(*contact);
	}
}

void CUserProfile::setWengoAccount(const WengoAccount & wengoAccount) {
	typedef ThreadEvent1<void (WengoAccount wengoAccount), WengoAccount> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::setWengoAccountThreadSafe, this, _1), wengoAccount);

	_modelThread.postEvent(event);
}

void CUserProfile::setWengoAccountThreadSafe(WengoAccount wengoAccount) {
	_userProfile.setWengoAccount(wengoAccount);
}

std::set<IMAccount *> CUserProfile::getIMAccountsOfProtocol(EnumIMProtocol::IMProtocol protocol) const {
	std::set<IMAccount *> result;

	for (IMAccountHandler::const_iterator it = _userProfile.getIMAccountHandler().begin();
		it != _userProfile.getIMAccountHandler().end();
		it++) {
		if ((*it).getProtocol() == protocol) {
			result.insert((IMAccount *)(&(*it)));
		}
	}

	return result;
}

PhoneCall * CUserProfile::getActivePhoneCall() const {
	PhoneCall * result = NULL;

	//FIXME: model must not be used directly by the GUI
	IPhoneLine * phoneLine = _userProfile.getActivePhoneLine();
	if (phoneLine) {
		result = phoneLine->getActivePhoneCall();
	}

	return result;
}
