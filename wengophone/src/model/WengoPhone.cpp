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
#include "WengoPhoneLogger.h"
#include "wenbox/WenboxPlugin.h"
#include "phoneline/PhoneLine.h"
#include "phoneline/IPhoneLine.h"
#include "phonecall/PhoneCall.h"
#include "account/SipAccount.h"
#include "account/wengo/WengoAccountXMLLayer.h"
#include "account/LocalNetworkAccount.h"
#include "contactlist/ContactList.h"
#include "contactlist/Contact.h"
#include "contactlist/ContactGroup.h"
#include "sipwrapper/SipWrapper.h"

#include <StringList.h>

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


WengoPhone::WengoPhone()
	: _contactList(ContactList::getInstance()) {
	_logger = new WengoPhoneLogger();
	_wenboxPlugin = new WenboxPlugin(*this);
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_terminate = false;
	_wengoAccountDataLayer = NULL;
}

WengoPhone::~WengoPhone() {
	delete _logger;
	delete _wenboxPlugin;
	delete _activePhoneLine;
	delete _activePhoneCall;
	delete _wengoAccountDataLayer;
}

void WengoPhone::init() {
	//Sends the logger creation event
	loggerCreatedEvent(*this, *_logger);

	//Creates the history
	//historyCreatedEvent

	//Creates the contact list
	contactListCreatedEvent(*this, _contactList);
	_contactList.load();

	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	//Loads SIP accounts
	WengoAccount * account = new WengoAccount(String::null, String::null, true);
	account->loginEvent += boost::bind(&WengoPhone::wengoLoginEventHandler, this, _1, _2, _3, _4);

	//Creates the connection to the IM services
	ConnectHandler * connectHandler = new ConnectHandler(*this, *account);
	connectHandlerCreatedEvent(*this, *connectHandler);

	_wengoAccountDataLayer = new WengoAccountXMLLayer(*account);
	bool noAccount = true;
	if (_wengoAccountDataLayer->load()) {
		if (account->hasAutoLogin()) {
			//Sends the HTTP request to the SSO
			account->init();
			noAccount = false;
		}
	}

	if (noAccount) {
		wengoLoginEvent(*this, LoginNoAccount, String::null, String::null);
	}

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
	return _contactList.toStringList();
}

void WengoPhone::terminate() {
	//Terminates the SIP stack
	if (_activePhoneLine) {
		_activePhoneLine->disconnect();
		_activePhoneLine->getSipWrapper().terminate();
	}

	_terminate = true;
}

void WengoPhone::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	typedef ThreadEvent3<void (const std::string &, const std::string &, bool), std::string, std::string, bool> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::addWengoAccountThreadSafe, this, _1, _2, _3), login, password, autoLogin);
	postEvent(event);
}

void WengoPhone::addWengoAccountThreadSafe(const std::string & login, const std::string & password, bool autoLogin) {
	//Creates a SipAccount
	WengoAccount * account = new WengoAccount(login, password, autoLogin);
	account->loginEvent += boost::bind(&WengoPhone::wengoLoginEventHandler, this, _1, _2, _3, _4);

	//Sends the HTTP request to the SSO
	account->init();
}

void WengoPhone::wengoLoginEventHandler(WengoAccount & sender, WengoAccount::LoginState state, const std::string & login, const std::string & password) {
	//Retrieval and parsing of the XML datas from the SSO has been done

	switch (state) {
	case WengoAccount::LoginOk:
		wengoLoginEvent(*this, LoginOk, login, password);
		delete _wengoAccountDataLayer;
		_wengoAccountDataLayer = new WengoAccountXMLLayer(sender);
		_wengoAccountDataLayer->save();

		addPhoneLine(sender);
		break;

	case WengoAccount::LoginNetworkError:
		wengoLoginEvent(*this, LoginNetworkError, login, password);
		break;

	case WengoAccount::LoginPasswordError:
		wengoLoginEvent(*this, LoginPasswordError, login, password);
		break;

	default:
		LOG_FATAL("WengoAccount::LoginState unknown state");
		break;
	}
}

void WengoPhone::addContact(Contact * contact, const std::string & contactGroupName) {
	typedef ThreadEvent2<void (Contact *, const std::string &), Contact *, std::string> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::addContactThreadSafe, this, _1, _2), contact, contactGroupName);
	postEvent(event);
}

void WengoPhone::addContactThreadSafe(Contact * contact, const std::string & contactGroupName) {
	ContactGroup * contactGroup = _contactList[contactGroupName];

	if (!contactGroup) {
		//Creates a new ContactGroup
		contactGroup = new ContactGroup(contactGroupName);
		_contactList.addContactGroup(contactGroup);
	}

	contactGroup->addContact(contact);

	LOG_DEBUG("contact added: " + contactGroup->toString() + " " + contact->toString());
}

void WengoPhone::addPhoneLine(const SipAccount & account) {
	//Creates new a PhoneLine associated with the account just added
	PhoneLine * phoneLine = new PhoneLine(*this);

	//Adds the PhoneLine to the list of PhoneLine
	_phoneLineList += phoneLine;

	//Sets the active PhoneLine
	//FIXME should the last added PhoneLine be the active one all the time?
	if (!_activePhoneLine) {
		//phoneLine.setActive(true);
		_activePhoneLine = phoneLine;
	}

	//Adds the account to the PhoneLine
	phoneLine->setSipAccount(account);

	//Sends the creation event
	phoneLineCreatedEvent(*this, *phoneLine);

	//Connects to the SIP server
	phoneLine->connect();
}

WenboxPlugin & WengoPhone::getWenboxPlugin() const {
	return *_wenboxPlugin;
}
