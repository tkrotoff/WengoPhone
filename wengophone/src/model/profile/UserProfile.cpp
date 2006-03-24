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
#include <model/connect/ConnectHandler.h>
#include <model/contactlist/ContactListFileStorage.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include <model/contactlist/IMContactListHandler.h>
#include <model/contactlist/ContactListStorage.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/presence/PresenceHandler.h>
#include <model/sms/Sms.h>

#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

using namespace std;

UserProfile::UserProfile(WengoPhone & wengoPhone) 
: _wengoPhone(wengoPhone) {
	_sms = NULL;
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_contactList = NULL;
	_wengoAccount = NULL;
	_wengoAccountDataLayer = NULL;
}

UserProfile::~UserProfile() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	IMAccountHandlerStorage * imAccountHandlerStorage = new IMAccountHandlerFileStorage(_imAccountHandler);
	imAccountHandlerStorage->save(config.getConfigDir() + "imaccounts.xml");
	delete imAccountHandlerStorage;

	if (_contactList) {
		ContactListStorage * contactListStorage = new ContactListFileStorage(*_contactList, _imAccountHandler);
		contactListStorage->save(config.getConfigDir() + "contactlist.xml");
		delete contactListStorage;
	}

	if (_sms) {
		delete _sms;
	}

	if (_activePhoneLine) {
		delete _activePhoneLine;
	}

	if (_activePhoneCall) {
		delete _activePhoneCall;
	}

	if (_contactList) {
		delete _contactList;
	}
}

void UserProfile::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Creates ConnectHandler, PresenceHandler, ChatHandler and IMContactListHandler
	_connectHandler = new ConnectHandler(*this);
	connectHandlerCreatedEvent(*this, *_connectHandler);

	_presenceHandler = new PresenceHandler(*this);
	presenceHandlerCreatedEvent(*this, *_presenceHandler);

	_chatHandler = new ChatHandler(*this);
	chatHandlerCreatedEvent(*this, *_chatHandler);

	_imContactListHandler = new IMContactListHandler(*this);
	////

	//Loads IMAccounts
	IMAccountHandlerStorage * imAccountHandlerStorage = new IMAccountHandlerFileStorage(_imAccountHandler);
	imAccountHandlerStorage->load(config.getConfigDir() + "imaccounts.xml");
	delete imAccountHandlerStorage;
	////

	// Loads SipAccounts
	////

	//Creates and loads the contact list
	_contactList = new ContactList(*this);
	contactListCreatedEvent(*this, *_contactList);

	ContactListStorage * contactListStorage = new ContactListFileStorage(*_contactList, _imAccountHandler);
	contactListStorage->load(config.getConfigDir() + "contactlist.xml");
	delete contactListStorage;	
	////
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
			_connectHandler->connect(*it);
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
		sleep(5);
		_activePhoneLine->getSipWrapper().terminate();
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
	imContactSet.insert(*imContact);
	_chatHandler->createSession(imContact->getIMAccount(), imContactSet);
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
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
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
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
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

}

void UserProfile::setAlias(const string & alias) {

}

string UserProfile::getAlias() const {
	return "User";
}

void UserProfile::addPhoneLine(SipAccount & account) {
	//Creates new a PhoneLine associated with the account just added
	PhoneLine * phoneLine = new PhoneLine(account, _wengoPhone
	);

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
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);
	_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	_wengoAccountDataLayer = new WengoAccountXMLLayer(*(WengoAccount *)_wengoAccount);
	bool noAccount = true;
	if (_wengoAccountDataLayer->load()) {
		if (_wengoAccount->hasAutoLogin()) {
			//Sends the HTTP request to the SSO
			_wengoAccount->init();
			noAccount = false;
		}
	}

	if (noAccount) {
		noAccountAvailableEvent(*this);
	}
}

void UserProfile::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	switch (state) {
	case SipAccount::LoginStateReady: {
		LOG_DEBUG("SMS created");
		//Creates SMS, SMS needs a WengoAccount
		_sms = new Sms(*(WengoAccount *) _wengoAccount);
		smsCreatedEvent(*this, *_sms);

		addPhoneLine(sender);

		if (_wengoAccountDataLayer) {
			delete _wengoAccountDataLayer;
		}

		_wengoAccountDataLayer = new WengoAccountXMLLayer(*(WengoAccount *) _wengoAccount);
		_wengoAccountDataLayer->save();

		IMAccount imAccount(_wengoAccount->getIdentity(), _wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		addIMAccount(imAccount);
		_connectHandler->connect(*_imAccountHandler.find(imAccount));

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
