/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006 Wengo
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

#include "UserProfile.h"

#include <model/WengoPhone.h>
#include <model/account/NetworkObserver.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/account/wengo/WengoAccountFileStorage.h>
#include <model/chat/ChatHandler.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phoneline/PhoneLineState.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/history/History.h>
#include <model/webservices/sms/WsSms.h>
#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <model/webservices/info/WsInfo.h>
#include <model/webservices/directory/WsDirectory.h>

#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <thread/Thread.h>

#include <exception>

using namespace std;

UserProfile::UserProfile(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone),
	_imContactListHandler(*this),
	_connectHandler(*this),
	_presenceHandler(*this),
	_chatHandler(*this),
	_contactList(*this),
	_modelThread(wengoPhone) {

	_wsSms = NULL;
	_wsInfo = NULL;
	_wsDirectory = NULL;
	_wsCallForward = NULL;
	_wsSoftUpdate = NULL;
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_wengoAccount = NULL;
	_imAccountHandler = new IMAccountHandler();
	_presenceState = EnumPresenceState::PresenceStateOffline;
	_wengoAccountConnected = false;
	_mustConnectWengoAccount = false;

	_history = new History(*this);
	_history->mementoUpdatedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
	_history->mementoAddedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
	_history->mementoRemovedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);

	_connectHandler.connectedEvent +=
		boost::bind(&UserProfile::connectedEventHandler, this, _1, _2);

	NetworkObserver::getInstance().connectionIsDownEvent +=
		boost::bind(&UserProfile::connectionIsDownEventHandler, this, _1);

	NetworkObserver::getInstance().connectionIsUpEvent +=
		boost::bind(&UserProfile::connectionIsUpEventHandler, this, _1);
}

UserProfile::~UserProfile() {
	disconnect();
	if (_activePhoneLine) {
		Thread::sleep(5);
		_activePhoneLine->getSipWrapper().terminate();
	}

	if (_activePhoneLine) {
		delete _activePhoneLine;
	}

	if (_activePhoneCall) {
		delete _activePhoneCall;
	}

	if (_history) {
		delete _history;
	}

	if (_imAccountHandler) {
		delete _imAccountHandler;
	}

	if (_wsSms) {
		delete _wsSms;
	}

	if (_wsSoftUpdate) {
		delete _wsSoftUpdate;
	}

	if (_wsInfo) {
		delete _wsInfo;
	}

	if (_wsDirectory) {
		delete _wsDirectory;
	}

	if (_wsCallForward) {
		delete _wsCallForward;
	}
}

void UserProfile::init() {
	_mustConnectWengoAccount = true;
	wengoAccountInit();
}

void UserProfile::connect() {
	connectIMAccounts();
	connectSipAccounts();
}

void UserProfile::connectIMAccounts() {
	//Connects all IMAccounts
	for (IMAccountHandler::const_iterator it = _imAccountHandler->begin();
		it != _imAccountHandler->end();
		++it) {
		newIMAccountAddedEvent(*this, (IMAccount &)*it);
		//FIXME: hack for phApi connection
		if ((*it).getProtocol() != EnumIMProtocol::IMProtocolSIPSIMPLE) {
			if ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline) {
				_connectHandler.connect((IMAccount &)*it);
			}
		}
	}
	////
}

void UserProfile::connectSipAccounts() {

	// Connect all SipAccounts if not connected
	if (_wengoAccount && _activePhoneLine && !_wengoAccountConnected) {
		_activePhoneLine->connect();
		_wengoAccountConnected = true;

		IMAccount imAccount(_wengoAccount->getIdentity(),
			_wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		addIMAccount(imAccount);
		_connectHandler.connect((IMAccount &)*_imAccountHandler->find(imAccount));
	}
	////
}

void UserProfile::disconnect() {
	disconnectIMAccounts();
	disconnectSipAccounts();
}

void UserProfile::disconnectIMAccounts() {
	for (IMAccountHandler::const_iterator it = _imAccountHandler->begin();
		it != _imAccountHandler->end();
		++it) {
		_connectHandler.disconnect((IMAccount &)*it);
	}
}

void UserProfile::disconnectSipAccounts(bool now) {
	if (_activePhoneLine && _wengoAccountConnected) {
		_activePhoneLine->disconnect(now);
		IMAccount imAccount(_wengoAccount->getIdentity(),
			_wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		_connectHandler.disconnect((IMAccount &)*_imAccountHandler->find(imAccount), now);
	}
}

void UserProfile::makeCall(Contact & contact, bool enableVideo) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(contact.getPreferredNumber(), enableVideo);
	}
}

void UserProfile::makeCall(const std::string & phoneNumber, bool enableVideo) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(phoneNumber, enableVideo);
	}
}

void UserProfile::startIM(Contact & contact) {
	IMContactSet imContactSet;
	IMContact * imContact = contact.getPreferredIMContact();

	//FIXME: we should give a set of pointer to IMContacts
	if (imContact) {
		imContactSet.insert(*imContact);
		_chatHandler.createSession((IMAccount &)*imContact->getIMAccount(), imContactSet);
	} else {
		LOG_ERROR("There is no IMContact available");
	}
}

void UserProfile::setWengoAccount(const WengoAccount & wengoAccount) {
	//TODO allow several SipAccount
	if (_wengoAccount) {
		IPhoneLine * phoneLine = findWengoPhoneLine();
		if (phoneLine) {
			phoneLine->disconnect();
			//TODO remove the PhoneLine from _phoneLines & destroy it
		}
		delete _wengoAccount;
		_wengoAccount = NULL;
	}

	_wengoAccount = new WengoAccount(wengoAccount);

	//Empty login or password
	if (_wengoAccount->getWengoLogin().empty() || _wengoAccount->getWengoPassword().empty()) {
		loginStateChangedEvent(*_wengoAccount, SipAccount::LoginStatePasswordError);
		return;
	}

	_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
	_wengoAccount->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);
	_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	//Sends the HTTP request to the SSO
	_mustConnectWengoAccount = true;
	_wengoAccount->init();
}

void UserProfile::addIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("adding an IMAccount");

	pair<IMAccountHandler::const_iterator, bool> result = _imAccountHandler->insert(imAccount);

	IMAccountHandler::const_iterator it = _imAccountHandler->find(imAccount);

	if (it == _imAccountHandler->end()) {
		LOG_FATAL("Error while inserting this IMAccount: " + imAccount.getLogin());
	}

	if (result.second) {
		newIMAccountAddedEvent(*this, (IMAccount &)*it);
	}
}

void UserProfile::removeIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("removing an IMAccount");

	IMAccountHandler::iterator it = _imAccountHandler->find(imAccount);
	if (it != _imAccountHandler->end()) {
		_connectHandler.disconnect((IMAccount &)*it);
		imAccountRemovedEvent(*this, (IMAccount &)*it);
		_imAccountHandler->erase(it);
	} else {
		LOG_ERROR("IMAccount not in IMAccountHandler");
	}
}

void UserProfile::setPresenceState(EnumPresenceState::PresenceState presenceState, IMAccount * imAccount) {
	_presenceState = presenceState;
	_presenceHandler.changeMyPresenceState(presenceState, String::null, imAccount);
}

void UserProfile::setAlias(const string & alias, IMAccount * imAccount) {
	_alias = alias;
	_presenceHandler.changeMyAlias(_alias, imAccount);
}

void UserProfile::addPhoneLine(SipAccount & account) {
	//Creates new a PhoneLine associated with the account just added
	PhoneLine * phoneLine = new PhoneLine(account, _wengoPhone);

	//Adds the PhoneLine to the list of PhoneLine
	_phoneLineList += phoneLine;

	//Sets the active PhoneLine
	//FIXME should the last added PhoneLine be the active one all the time?
	if (!_activePhoneLine) {
		//phoneLine.setActive(true);
		_activePhoneLine = phoneLine;
	}

	//Sends the creation event
	phoneLineCreatedEvent(*this, *phoneLine);
}

void UserProfile::wengoAccountInit() {
	if (!_wengoAccount) {
		_wengoAccount = new WengoAccount(String::null, String::null, true);
		_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
		_wengoAccount->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
		_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);
		_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
		_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

		Config & config = ConfigManager::getInstance().getCurrentConfig();

		//FIXME Needs to be dynamic?
		WengoAccountFileStorage * wengoAccountFileStorage = new WengoAccountFileStorage(*(WengoAccount *) _wengoAccount);
		bool noAccount = true;
		if (wengoAccountFileStorage->load(config.getConfigDir())) {
			if (_wengoAccount->hasAutoLogin()) {
				//Sends the HTTP request to the SSO
				_wengoAccount->init();
				noAccount = false;
			}
		}
		delete wengoAccountFileStorage;

		if (noAccount) {
			noAccountAvailableEvent(*this);
		}
	}
}

void UserProfile::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	switch (state) {
	case SipAccount::LoginStateReady: {
		//Creates SMS, SMS needs a WengoAccount
		_wsSms = new WsSms(_wengoAccount, *this);
		wsSmsCreatedEvent(*this, *_wsSms);
		LOG_DEBUG("SMS created");

		//Creates SoftUpdate, SoftUpdate needs a WengoAccount
		_wsSoftUpdate = new WsSoftUpdate(_wengoAccount);
		wsSoftUpdateCreatedEvent(*this, *_wsSoftUpdate);
		_wsSoftUpdate->checkForUpdate();
		LOG_DEBUG("SoftUpdate created");

		_wsInfo = new WsInfo(_wengoAccount);
		wsInfoCreatedEvent(*this, *_wsInfo);
		LOG_DEBUG("WsInfo created");

		//WsDirectory
		_wsDirectory = new WsDirectory(_wengoAccount);
		wsDirectoryCreatedEvent(*this, *_wsDirectory);
		LOG_DEBUG("WsDirectory created");

		//TODO: callforward
		_wsCallForward = new WsCallForward(_wengoAccount);
		wsCallForwardCreatedEvent(*this, *_wsCallForward);
		_wsCallForward->wsCallForwardEvent += boost::bind(&UserProfile::wsCallForwardEventHandler, this, _1, _2, _3);

		Config & config = ConfigManager::getInstance().getCurrentConfig();

		//FIXME Needs to be dynamic?
		WengoAccountFileStorage * wengoAccountFileStorage = new WengoAccountFileStorage(*(WengoAccount *) _wengoAccount);
		wengoAccountFileStorage->save(config.getConfigDir());
		delete wengoAccountFileStorage;

		addPhoneLine(*_wengoAccount);

		if (_mustConnectWengoAccount) {
			_mustConnectWengoAccount = false;
			connectSipAccounts();
		}

		loadHistory();

		break;
	}

	case SipAccount::LoginStateConnected: {
		_wengoAccountConnected = true;
	}

	case SipAccount::LoginStateDisconnected: {
		_wengoAccountConnected = false;
	}

	default:
		;
	}
}

IPhoneLine * UserProfile::findWengoPhoneLine() {
	for (unsigned i = 0; i != _phoneLineList.size() ; i++) {
		const SipAccount & sipAccount = _phoneLineList[i]->getSipAccount();
		try {
			const WengoAccount & wengoAccount = dynamic_cast<const WengoAccount &>(sipAccount);
			return _phoneLineList[i];
		} catch (bad_cast) {
			LOG_ERROR("WengoAccount dynamic_cast failed");
		}
	}
	return NULL;
}

void UserProfile::historyChangedEventHandler(History & sender, int id) {
	saveHistory();
}

void UserProfile::setIcon(const Picture & icon, IMAccount * imAccount) {
	Profile::setIcon(icon);
	_presenceHandler.changeMyIcon(icon, imAccount);
}

void UserProfile::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	_presenceState = imAccount.getPresenceState();
	_presenceHandler.changeMyPresenceState(imAccount.getPresenceState(), String::null, &imAccount);
	_presenceHandler.changeMyAlias(_alias, NULL);
	_presenceHandler.changeMyIcon(_icon, NULL);
}

void UserProfile::loadHistory() {
	//History: load the user history
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string filename = config.getConfigDir() + _wengoAccount->getIdentity() + "_history";
	_history->load(filename);
}

void UserProfile::saveHistory() {
	//History: save the history
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string filename = config.getConfigDir() + _wengoAccount->getIdentity() + "_history";
	_history->save(filename);
}

bool UserProfile::hasWengoAccount() const {
	return _wengoAccount != NULL;
}

void UserProfile::wsCallForwardEventHandler(WsCallForward & sender,
	int id, WsCallForward::WsCallForwardStatus status) {

	if (status == WsCallForward::WsCallForwardStatusOk) {

		_wsInfo->getWengosCount(false);
		_wsInfo->getSmsCount(false);
		_wsInfo->getActiveMail(false);
		_wsInfo->getUnreadVoiceMail(false);
		_wsInfo->getPstnNumber(false);
		_wsInfo->getCallForwardInfo(true);
		_wsInfo->execute();
	}
}

void UserProfile::connectionIsUpEventHandler(NetworkObserver & sender) {
	connectSipAccounts();
}

void UserProfile::connectionIsDownEventHandler(NetworkObserver & sender) {
	disconnectSipAccounts(true);
}
