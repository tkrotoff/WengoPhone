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
#include <control/webservices/sms/CSms.h>
#include <control/webservices/softupdate/CSoftUpdate.h>
#include <control/webservices/subscribe/CSubscribe.h>
#include <control/webservices/directory/CWsDirectory.h>
#include <control/webservices/callforward/CWsCallForward.h>

#include <util/Logger.h>

using namespace std;

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_cUserProfile = new CUserProfile(_wengoPhone.getCurrentUserProfile(), wengoPhone);

	_wengoPhone.getCurrentUserProfile().loginStateChangedEvent += loginStateChangedEvent;
	_wengoPhone.getCurrentUserProfile().networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	_wengoPhone.getCurrentUserProfile().noAccountAvailableEvent += noAccountAvailableEvent;
	_wengoPhone.getCurrentUserProfile().proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoPhone.getCurrentUserProfile().wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	_wengoPhone.wenboxPluginCreatedEvent +=
		boost::bind(&CWengoPhone::wenboxPluginCreatedEventHandler, this, _1, _2);

	_wengoPhone.wsSubscribeCreatedEvent +=
		boost::bind(&CWengoPhone::wsSubscribeCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().wsDirectoryCreatedEvent +=
		boost::bind(&CWengoPhone::wsDirectoryCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().wsSmsCreatedEvent +=
		boost::bind(&CWengoPhone::wsSmsCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().wsSoftUpdateCreatedEvent +=
		boost::bind(&CWengoPhone::wsSoftUpdateCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().wsCallForwardCreatedEvent +=
		boost::bind(&CWengoPhone::wsCallForwardCreatedEventHandler, this, _1, _2);

	_wengoPhone.initFinishedEvent +=
		boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.getCurrentUserProfile().phoneLineCreatedEvent +=
		boost::bind(&CWengoPhone::phoneLineCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().newIMAccountAddedEvent +=
		boost::bind(&CWengoPhone::newIMAccountAddedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().getHistory().historyLoadedEvent +=
		boost::bind(&CWengoPhone::historyLoadedEventHandler, this, _1);

//	_wengoPhone.timeoutEvent += boost::bind(&CWengoPhone::controlssoTimeoutEventHandler, this);
	_wengoPhone.timeoutEvent += controlTimeoutEvent;
	_wengoPhone.getCurrentUserProfile().getPresenceHandler().authorizationRequestEvent +=
		boost::bind(&CWengoPhone::authorizationRequestEventHandler, this, _1, _2, _3);

	_cWsCallForward = NULL;
}

void CWengoPhone::start() {
	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	CPhoneLine * cPhoneLine = new CPhoneLine(phoneLine, *this);

	LOG_DEBUG("CPhoneLine created");
}

void CWengoPhone::wenboxPluginCreatedEventHandler(WengoPhone & sender, WenboxPlugin & wenboxPlugin) {
	static CWenboxPlugin cWenboxPlugin(wenboxPlugin, *this);

	LOG_DEBUG("CWenboxPlugin created");
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	static CChatHandler cChatHandler(sender.getCurrentUserProfile().getChatHandler(), *this,sender.getCurrentUserProfile());
	LOG_DEBUG("CChatHandler created");

	_pWengoPhone->modelInitializedEvent();

	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::wsSubscribeCreatedEventHandler(WengoPhone & sender, WsSubscribe & wsSubscribe) {
	static CSubscribe cSubscribe(wsSubscribe, *this);

	LOG_DEBUG("CWenboxPlugin created");
}

void CWengoPhone::wsDirectoryCreatedEventHandler(UserProfile & sender, WsDirectory & wsDirectory) {
	static CWsDirectory cWsDirectory(*this, wsDirectory);

	LOG_DEBUG("CWsDirectory created");
}

void CWengoPhone::wsSmsCreatedEventHandler(UserProfile & sender, WsSms & wsSms) {
	static CSms cSms(wsSms, *this);

	LOG_DEBUG("CSms created");
}

void CWengoPhone::wsCallForwardCreatedEventHandler(UserProfile & sender, WsCallForward & wsCallForward) {
	_cWsCallForward = new CWsCallForward(*this, wsCallForward);
}

void CWengoPhone::wsSoftUpdateCreatedEventHandler(UserProfile & sender, WsSoftUpdate & wsSoftUpdate) {
	static CSoftUpdate cSoftUpdate(wsSoftUpdate, *this);

	LOG_DEBUG("CSoftUpdate created");
}

PhoneCall * CWengoPhone::getActivePhoneCall() const {
	IPhoneLine * phoneLine = _wengoPhone.getCurrentUserProfile().getActivePhoneLine();
	if (phoneLine) {
		return phoneLine->getActivePhoneCall();
	}
	return NULL;
}

void CWengoPhone::addIMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol) {
	_wengoPhone.getCurrentUserProfile().addIMAccount(IMAccount(login, password, protocol));
}

void CWengoPhone::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	//_wengoPhone.getCurrentUserProfile().getConnectHandler().connect(imAccount);
}

void CWengoPhone::historyLoadedEventHandler(History & history) {
	_cHistory = new CHistory(history, *this, _wengoPhone);
	cHistoryCreatedEvent(*this, *_cHistory);
}

void CWengoPhone::authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
	const std::string & message) {
	_pWengoPhone->authorizationRequestEventHandler(sender, imContact, message);
}

void CWengoPhone::saveUserProfile() {
	_wengoPhone.saveUserProfile();
}
