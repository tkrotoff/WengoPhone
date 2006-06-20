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
#include <model/account/wengo/WengoAccount.h>
#include <model/chat/ChatHandler.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include <model/history/History.h>
#include <model/network/NetworkObserver.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phoneline/PhoneLineState.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/history/History.h>
#include <model/webservices/sms/WsSms.h>
#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <model/webservices/info/WsInfo.h>
#include <model/webservices/directory/WsDirectory.h>
#include <model/wenbox/WenboxPlugin.h>

#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <sipwrapper/SipWrapper.h>

#include <util/File.h>
#include <util/Logger.h>
#include <thread/Thread.h>

#include <exception>

using namespace std;

const std::string UserProfile::DEFAULT_USERPROFILE_NAME = "Default";

UserProfile::UserProfile(Thread & modelThread)
	: _modelThread(modelThread),
	_imContactListHandler(*this),
	_connectHandler(*this, modelThread),

	_presenceHandler(*this),
	_chatHandler(*this),
	_contactList(*this) {

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
	_wengoAccountInitializationFinished = false;
	_wengoAccountIsValid = false;
	_wengoAccountMustConnectAfterInit = false;
	_wenboxPlugin = NULL;

	_history = new History(*this);
	_history->mementoUpdatedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
	_history->mementoAddedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
	_history->mementoRemovedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);

	_connectHandler.connectedEvent +=
		boost::bind(&UserProfile::connectedEventHandler, this, _1, _2);

	computeName();

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

	if (_wenboxPlugin) {
		delete _wenboxPlugin;
	}
}

void UserProfile::init() {
	_wenboxPlugin = new WenboxPlugin(*this);

	_wengoAccountMustConnectAfterInit = true;
	wengoAccountInit();
}

std::string UserProfile::getProfileDirectory() const {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return File::convertPathSeparators(config.getConfigDir() + "profiles/" + _name + "/");
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
		_wengoAccountConnected = false;
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

int UserProfile::makeCall(Contact & contact, bool enableVideo) {
	if (_activePhoneLine) {
		return _activePhoneLine->makeCall(contact.getPreferredNumber(), enableVideo);
	}
}

int UserProfile::makeCall(const std::string & phoneNumber, bool enableVideo) {
	if (_activePhoneLine) {
		return _activePhoneLine->makeCall(phoneNumber, enableVideo);
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

	if (!wengoAccount.getWengoLogin().empty()) {
		_wengoAccountInitializationFinished = false;
		_wengoAccount = new WengoAccount(wengoAccount);
		computeName();

		//Empty login or password
		if (_wengoAccount->getWengoLogin().empty() || _wengoAccount->getWengoPassword().empty()) {
			loginStateChangedEvent(*_wengoAccount, SipAccount::LoginStatePasswordError);
			return;
		}

		_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
		_wengoAccount->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
		_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);

		//Sends the HTTP request to the SSO
		_wengoAccount->init();
	}
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
	PhoneLine * phoneLine = new PhoneLine(account, *this);

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
	if (_wengoAccount) {
		if (_wengoAccount->hasAutoLogin()) {
			_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
			_wengoAccount->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
			_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);

			//Sends the HTTP request to the SSO
			_wengoAccount->init();
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

		addPhoneLine(*_wengoAccount);

		IMAccount imAccount(_wengoAccount->getIdentity(),
			_wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		addIMAccount(imAccount);

		loadHistory();

		_wengoAccountIsValid = true;
		_wengoAccountInitializationFinished = true;

		if (_wengoAccountMustConnectAfterInit) {
			_wengoAccountMustConnectAfterInit = false;
			//FIXME: currently there is only one sip account and we are sure
			//this is a Wengo account.
			connectSipAccounts();
		}

		break;
	}

	case SipAccount::LoginStatePasswordError: {
		_wengoAccountIsValid = false;
		_wengoAccountInitializationFinished = true;

		break;
	}

	case SipAccount::LoginStateConnected: {
		_wengoAccountConnected = true;
		break;
	}

	case SipAccount::LoginStateDisconnected: {
		_wengoAccountConnected = false;
		break;
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
    _history->load(getProfileDirectory() + "history.xml");
    historyLoadedEvent(*this, *_history);
}

void UserProfile::saveHistory() {
    _history->save(getProfileDirectory() + "history.xml");
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

void UserProfile::computeName() {
	if (!_wengoAccount || _wengoAccount->getWengoLogin().empty()) {
		_name = DEFAULT_USERPROFILE_NAME;
	} else {
		_name = _wengoAccount->getWengoLogin();
	}
}

bool UserProfile::isWengoAccountValid() {
	bool result = false;

	if (!hasWengoAccount()) {
		result = true;
	} else {
		// Waiting for end of Wengo initialization
		while (!_wengoAccountInitializationFinished) {
			Thread::msleep(100);
		}

		result = _wengoAccountIsValid;
	}

	return result;
}

void UserProfile::connectionIsUpEventHandler(NetworkObserver & sender) {
	connectSipAccounts();
}

void UserProfile::connectionIsDownEventHandler(NetworkObserver & sender) {
	disconnectSipAccounts(true);
}
