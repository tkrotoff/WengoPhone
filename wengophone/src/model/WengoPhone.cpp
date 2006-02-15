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
#include "contactlist/ContactList.h"
#include "contactlist/Contact.h"
#include "contactlist/ContactGroup.h"
#include "config/ConfigManager.h"
#include "config/Config.h"
#include "chat/ChatHandler.h"
#include "presence/PresenceHandler.h"
#include "connect/ConnectHandler.h"

#include <sipwrapper/SipWrapper.h>

#include <StringList.h>
#include <Logger.h>

#include <iostream>
using namespace std;

std::string WengoPhone::HTTP_PROXY_HOSTNAME = "";
unsigned WengoPhone::HTTP_PROXY_PORT = 0;
std::string WengoPhone::CONFIG_FILES_PATH = "";
std::string WengoPhone::getConfigFilesPath() {
	/*std::string tmp = File::getApplicationDirPath();
	return tmp;*/
	return CONFIG_FILES_PATH;
}


WengoPhone::WengoPhone() {
	_wenboxPlugin = new WenboxPlugin(*this);
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_terminate = false;
	_wengoAccount = NULL;
	_wengoAccountDataLayer = NULL;
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
}

void WengoPhone::init() {
	//Creates the history
	//historyCreatedEvent

	//Create ConnectHandler, PresenceHandler and ChatHandler
	_connectHandler = new ConnectHandler();
	connectHandlerCreatedEvent(*this, *_connectHandler);

	_presenceHandler = new PresenceHandler(*this);
	presenceHandlerCreatedEvent(*this, *_presenceHandler);

	_chatHandler = new ChatHandler(*this);
	chatHandlerCreatedEvent(*this, *_chatHandler);

	//Creates the contact list
	_contactList = new ContactList(*this);
	contactListCreatedEvent(*this, *_contactList);
	_contactList->load();

	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	//Load the WengoAccount
	wengoAccountLogin();

	//Load other SipAccount

	//Load IMAccounts
	//

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

void WengoPhone::makeCall(const std::string & phoneNumber) {
	if (_activePhoneLine) {
		_activePhoneLine->makeCall(phoneNumber);
	}
}

StringList WengoPhone::getContactGroupStringList() const {
	return _contactList->toStringList();
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
	//TODO: allow several SipAccount
	if (_wengoAccount) {
		//TODO: destroy the PhoneLine if it exists
		delete _wengoAccount;
	}

	_wengoAccount = new WengoAccount(login, password, autoLogin);
	_wengoAccount->loginStateChangedEvent += loginStateChangedEvent;
	_wengoAccount->networkDiscoveryStateEvent += networkDiscoveryStateEvent;
	_wengoAccount->loginStateChangedEvent += boost::bind(&WengoPhone::loginStateChangedEventHandler, this, _1, _2);
	_wengoAccount->proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoAccount->wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;

	//Sends the HTTP request to the SSO
	_wengoAccount->init();
}

void WengoPhone::addContact(Contact * contact, const std::string & contactGroupName) {
	typedef ThreadEvent2<void (Contact *, const std::string &), Contact *, std::string> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::addContactThreadSafe, this, _1, _2), contact, contactGroupName);
	postEvent(event);
}

void WengoPhone::addContactThreadSafe(Contact * contact, const std::string & contactGroupName) {
	ContactGroup * contactGroup = (*_contactList)[contactGroupName];

	if (!contactGroup) {
		//Creates a new ContactGroup
		contactGroup = new ContactGroup(contactGroupName, *this);
		_contactList->addContactGroup(contactGroup);
	}

	contactGroup->addContact(contact);

	LOG_DEBUG("contact added: " + contactGroup->toString() + " " + contact->toString());
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
	case SipAccount::LoginStateReady:
		addPhoneLine(sender);

		if (_wengoAccountDataLayer) {
			delete _wengoAccountDataLayer;
		}

		_wengoAccountDataLayer = new WengoAccountXMLLayer(*(WengoAccount *)_wengoAccount);
		_wengoAccountDataLayer->save();

		IMAccount imAccount(_wengoAccount->getIdentity(), _wengoAccount->getPassword(), EnumIMProtocol::IMProtocolSIPSIMPLE);
		_imAccountHandler.insert(imAccount);
		_connectHandler->connect(*_imAccountHandler.find(imAccount));

		break;
	}
}

void WengoPhone::addIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("adding an IMAccount");

	_imAccountHandler.insert(imAccount);

	newIMAccountAddedEvent(*this, *_imAccountHandler.find(imAccount));
}
