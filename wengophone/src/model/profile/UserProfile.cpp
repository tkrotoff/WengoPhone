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

#include <model/account/wengo/WengoAccount.h>
#include <model/account/wengo/WengoAccountXMLLayer.h>
#include <model/chat/ChatHandler.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/ContactListFileStorage.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include <model/contactlist/ContactListStorage.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/webservices/sms/Sms.h>
#include <model/webservices/softupdate/SoftUpdate.h>
#include <model/history/History.h>

#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <thread/Thread.h>

using namespace std;

UserProfile::UserProfile(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone),
	_imContactListHandler(*this),
	_connectHandler(*this),
	_presenceHandler(*this),
	_chatHandler(*this),
	_contactList(*this) {

	_sms = NULL;
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_wengoAccount = NULL;
	_softUpdate = NULL;
	
	_history = new History(*this);
	_history->mementoUpdatedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
	_history->mementoAddedEvent += boost::bind(&UserProfile::historyChangedEventHandler, this, _1, _2);
}

UserProfile::~UserProfile() {
	if (_sms) {
		delete _sms;
	}

	if (_activePhoneLine) {
		delete _activePhoneLine;
	}

	if (_activePhoneCall) {
		delete _activePhoneCall;
	}

	if (_softUpdate) {
		delete _softUpdate;
	}
	
	if (_history) {
		delete _history;
	}
}

void UserProfile::connect() {
	connectIMAccounts();
	connectSipAccounts();
}

void UserProfile::connectIMAccounts() {
	//Connects all IMAccounts
	for (IMAccountHandler::const_iterator it = _imAccountHandler.begin();
		it != _imAccountHandler.end();
		++it) {
		newIMAccountAddedEvent(*this, (IMAccount &)*it);
		//FIXME: hack for phApi connection
		if ((*it).getProtocol() != EnumIMProtocol::IMProtocolSIPSIMPLE) {
			_connectHandler.connect(*it);
		}
	}
	////
}

void UserProfile::connectSipAccounts() {
	// Connect all SipAccounts
	wengoAccountLogin();
	////
}

void UserProfile::disconnect() {
	if (_activePhoneLine) {
		_activePhoneLine->disconnect();
		Thread::sleep(5);
		_activePhoneLine->getSipWrapper().terminate();
	}

	for (IMAccountHandler::const_iterator it = _imAccountHandler.begin();
		it != _imAccountHandler.end();
		++it) {
		_connectHandler.disconnect(*it);
	}
}

void UserProfile::makeCall(Contact & contact) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(contact.getPreferredNumber());
	}
}

void UserProfile::makeCall(const std::string & phoneNumber) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(phoneNumber);
	}
}

void UserProfile::startIM(Contact & contact) {
	IMContactSet imContactSet;
	IMContact * imContact = contact.getPreferredIMContact();

	//FIXME: we should give a set of pointer to IMContacts
	if (imContact) {
		imContactSet.insert(*imContact);
		_chatHandler.createSession(imContact->getIMAccount(), imContactSet);
	} else {
		LOG_ERROR("There is no IMContact available");
	}
}

void UserProfile::addSipAccount(const std::string & login, const std::string & password, bool autoLogin) {
/*
	typedef ThreadEvent3<void (const std::string & login, const std::string & password, bool autoLogin), std::string, std::string, bool> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&UserProfile::addSipAccountThreadSafe, this, _1, _2, _3), login, password, autoLogin);
	postEvent(event);
*/
	//TODO allow several SipAccount
	if (_wengoAccount) {
		IPhoneLine * p = findWengoPhoneLine();
		if( p ) {
			p->disconnect();
			//TODO remove the PhoneLine from _phoneLines & destroy it
		}
		delete _wengoAccount;
	}

	_wengoAccount = new WengoAccount(login, password, autoLogin);

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
	_wengoAccount->init();
}

void UserProfile::addSipAccountThreadSafe(const std::string & login, const std::string & password, bool autoLogin) {
/*
	//TODO allow several SipAccount
	if (_wengoAccount) {
		IPhoneLine * p = findWengoPhoneLine();
		if( p ) {
			p->disconnect();
			//TODO remove the PhoneLine from _phoneLines & destroy it
		}
		delete _wengoAccount;
	}

	_wengoAccount = new WengoAccount(login, password, autoLogin);

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
	_wengoAccount->init();
*/
}

void UserProfile::addIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("adding an IMAccount");

	pair<IMAccountHandler::const_iterator, bool> result = _imAccountHandler.insert(imAccount);

	IMAccountHandler::const_iterator it = _imAccountHandler.find(imAccount);

	if (it == _imAccountHandler.end()) {
		LOG_FATAL("Error while inserting this IMAccount: " + imAccount.getLogin());
	}

	if (result.second) {
		newIMAccountAddedEvent(*this, (IMAccount &)*it);
	}
}

EnumPresenceState::PresenceState UserProfile::getPresenceState() const {
	return EnumPresenceState::PresenceStateOnline;
}

void UserProfile::setPresenceState(EnumPresenceState::PresenceState presenceState) {
	_presenceHandler.changeMyPresenceState(presenceState, "");
}

void UserProfile::setAlias(const string & alias) {
	_alias = alias;
	_presenceHandler.changeMyAlias(_alias);
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

	//Connects to the SIP server
	phoneLine->connect();
}

void UserProfile::wengoAccountLogin() {
	_wengoAccount = new WengoAccount(String::null, String::null, true);
	_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
	_wengoAccount->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);
	_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	WengoAccountDataLayer * wengoAccountDataLayer = new WengoAccountXMLLayer(*(WengoAccount *)_wengoAccount);
	bool noAccount = true;
	if (wengoAccountDataLayer->load()) {
		if (_wengoAccount->hasAutoLogin()) {
			//Sends the HTTP request to the SSO
			_wengoAccount->init();
			noAccount = false;
		}
	}
	delete wengoAccountDataLayer;

	if (noAccount) {
		noAccountAvailableEvent(*this);
	}
}

void UserProfile::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	switch (state) {
	case SipAccount::LoginStateReady: {
		//Creates SMS, SMS needs a WengoAccount
		LOG_DEBUG("SMS created");
		_sms = new Sms(*(WengoAccount *) _wengoAccount, *this);
		smsCreatedEvent(*this, *_sms);

		//Creates SoftUpdate, SoftUpdate needs a WengoAccount
		LOG_DEBUG("SoftUpdate created");
		_softUpdate = new SoftUpdate(*(WengoAccount *) _wengoAccount);
		softUpdateCreatedEvent(*this, *_softUpdate);
		_softUpdate->checkForUpdate();

		addPhoneLine(sender);

		WengoAccountDataLayer * wengoAccountDataLayer = new WengoAccountXMLLayer(*(WengoAccount *) _wengoAccount);
		wengoAccountDataLayer->save();
		delete wengoAccountDataLayer;

		IMAccount imAccount(_wengoAccount->getIdentity(), _wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		addIMAccount(imAccount);
		_connectHandler.connect(*_imAccountHandler.find(imAccount));
		
		loadHistory();
		
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
		} catch ( bad_cast ) {
			LOG_DEBUG("WengoAccount dynamic_cast failed");
		}
	}
    return NULL;
}

void UserProfile::historyChangedEventHandler(History & sender, int id) {
	saveHistory();
}

void UserProfile::setIcon(const Picture & icon) {
	Profile::setIcon(icon);
	_presenceHandler.changeMyIcon(icon, NULL);
}

void UserProfile::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
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
