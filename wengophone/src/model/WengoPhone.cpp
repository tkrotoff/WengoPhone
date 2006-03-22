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

#include "WengoPhone.h"

#include "connect/ConnectHandler.h"
#include "wenbox/WenboxPlugin.h"
#include "phoneline/PhoneLine.h"
#include "phoneline/IPhoneLine.h"
#include "phonecall/PhoneCall.h"
#include "account/SipAccount.h"
#include "account/wengo/WengoAccount.h"
#include "account/wengo/WengoAccountXMLLayer.h"
//#include "account/LocalNetworkAccount.h"
#include "contactlist/ContactListFileStorage.h"
#include "contactlist/ContactList.h"
#include "contactlist/Contact.h"
#include "contactlist/ContactGroup.h"
#include "contactlist/IMContactListHandler.h"
#include "chat/ChatHandler.h"
#include "presence/PresenceHandler.h"
#include "connect/ConnectHandler.h"
#include "sms/Sms.h"
#include "config/ConfigManager.h"
#include "config/Config.h"

#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <sipwrapper/SipWrapper.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <iostream>
using namespace std;

WengoPhone::WengoPhone() {
	_wenboxPlugin = new WenboxPlugin(*this);
	_sms = NULL;
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_terminate = false;
	_wengoAccount = NULL;
	_wengoAccountDataLayer = NULL;
	_contactList = NULL;
}

WengoPhone::~WengoPhone() {
	delete _wenboxPlugin;
	delete _activePhoneLine;
	delete _activePhoneCall;

	if (_wengoAccountDataLayer) {
		delete _wengoAccountDataLayer;
	}

	if (_wengoAccount) {
		delete _wengoAccount;
	}

	if (_sms) {
		delete _sms;
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	IMAccountHandlerStorage * imAccountHandlerStorage = new IMAccountHandlerFileStorage(_imAccountHandler);
	imAccountHandlerStorage->save(config.getConfigDir() + "imaccounts.xml");
	delete imAccountHandlerStorage;

	if (_contactList) {
		ContactListStorage * contactListStorage = new ContactListFileStorage(*_contactList, _imAccountHandler);
		contactListStorage->save(config.getConfigDir() + "contactlist.xml");
		delete contactListStorage;
	}
}

void WengoPhone::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Creates the history
	//historyCreatedEvent

	//Create ConnectHandler, PresenceHandler, ChatHandler
	// and IMContactListHandler
	_connectHandler = new ConnectHandler(*this);
	connectHandlerCreatedEvent(*this, *_connectHandler);

	_presenceHandler = new PresenceHandler(*this);
	presenceHandlerCreatedEvent(*this, *_presenceHandler);

	_chatHandler = new ChatHandler(*this);
	chatHandlerCreatedEvent(*this, *_chatHandler);

	_imContactListHandler = new IMContactListHandler(*this);

	//Loads IMAccounts
	IMAccountHandlerStorage * imAccountHandlerStorage = new IMAccountHandlerFileStorage(_imAccountHandler);
	imAccountHandlerStorage->load(config.getConfigDir() + "imaccounts.xml");
	delete imAccountHandlerStorage;
	////

	//Creates and loads the contact list
	_contactList = new ContactList(*this);
	contactListCreatedEvent(*this, *_contactList);

	ContactListStorage * contactListStorage = new ContactListFileStorage(*_contactList, _imAccountHandler);
	contactListStorage->load(config.getConfigDir() + "contactlist.xml");
	delete contactListStorage;	
	////

	//Connects all IMAccounts
	for (IMAccountHandler::const_iterator it = _imAccountHandler.begin();
		 it != _imAccountHandler.end();
		 ++it) {
		newIMAccountAddedEvent(*this, (IMAccount &)*it);
		_connectHandler->connect(*it);
	}
	////

	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	//Load the WengoAccount
	wengoAccountLogin();

	//Load other SipAccount

	//initFinishedEvent
	initFinishedEvent(*this);
}

void WengoPhone::run() {
	init();
	LOG_DEBUG("The model thread is ready for events");

	//Keeps the thread running until terminate() is called
	while (!_terminate) {
		runEvents();
	}
}

void WengoPhone::makeCall(Contact & contact) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(contact.getPreferredNumber());
	}
}

void WengoPhone::makeCall(const std::string & phoneNumber) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(phoneNumber);
	}
}

void WengoPhone::startIM(Contact & contact) {
	IMContactSet imContactSet;
	IMContact * imContact = contact.getPreferredIMContact();

	//FIXME: we should give a set of pointer to IMContacts
	imContactSet.insert(*imContact);
	_chatHandler->createSession(imContact->getIMAccount(), imContactSet);
}

void WengoPhone::terminate() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::terminateThreadSafe, this));
	postEvent(event);
}

void WengoPhone::terminateThreadSafe() {
	//Terminates the SIP stack
	if (_activePhoneLine) {
		_activePhoneLine->disconnect();
		sleep(5);
		_activePhoneLine->getSipWrapper().terminate();
	}

	_terminate = true;
}

void WengoPhone::addSipAccount(const std::string & login, const std::string & password, bool autoLogin) {
	typedef ThreadEvent3<void (const std::string &, const std::string &, bool), std::string, std::string, bool> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::addSipAccountThreadSafe, this, _1, _2, _3), login, password, autoLogin);
	postEvent(event);
}

void WengoPhone::addSipAccountThreadSafe(const std::string & login, const std::string & password, bool autoLogin) {
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
	if (login.empty() || password.empty()) {
		loginStateChangedEvent(*_wengoAccount, SipAccount::LoginStatePasswordError);
		return;
	}

	_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&WengoPhone::loginStateChangedEventHandler, this, _1, _2);
	_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	//Sends the HTTP request to the SSO
	_wengoAccount->init();
}

void WengoPhone::addPhoneLine(SipAccount & account) {
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

	//Connects to the SIP server
	phoneLine->connect();
}

WenboxPlugin & WengoPhone::getWenboxPlugin() const {
	return *_wenboxPlugin;
}

void WengoPhone::wengoAccountLogin() {
	_wengoAccount = new WengoAccount(String::null, String::null, true);
	_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&WengoPhone::loginStateChangedEventHandler, this, _1, _2);
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

void WengoPhone::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
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

void WengoPhone::addIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("adding an IMAccount");

	_imAccountHandler.insert(imAccount);

	IMAccountHandler::const_iterator it = _imAccountHandler.find(imAccount);

	if (it == _imAccountHandler.end()) {
		LOG_FATAL("Error while inserting this IMAccount: " + imAccount.getLogin());
	}

	newIMAccountAddedEvent(*this, (IMAccount &)*it);
}

IPhoneLine * WengoPhone::findWengoPhoneLine() {
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
