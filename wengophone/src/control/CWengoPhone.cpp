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

#include "CWengoPhone.h"

#include "model/WengoPhoneLogger.h"
#include "model/wenbox/WenboxPlugin.h"
#include "model/account/wengo/WengoAccount.h"
#include "model/contactlist/Contact.h"
#include "presentation/PFactory.h"
#include "presentation/PWengoPhone.h"
#include "model/phoneline/IPhoneLine.h"
#include "phoneline/CPhoneLine.h"
#include "contactlist/CContactList.h"
#include "CWengoPhoneLogger.h"
#include "wenbox/CWenboxPlugin.h"
#include "WengoPhoneBuildId.h"
#include "CConnectHandler.h"

#include <WebBrowser.h>
#include <StringList.h>

using namespace std;

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_wengoPhone.phoneLineCreatedEvent += boost::bind(&CWengoPhone::phoneLineCreatedEventHandler, this, _1, _2);
	_wengoPhone.loggerCreatedEvent += boost::bind(&CWengoPhone::loggerCreatedEventHandler, this, _1, _2);
	_wengoPhone.wenboxPluginCreatedEvent += boost::bind(&CWengoPhone::wenboxPluginCreatedEventHandler, this, _1, _2);
	_wengoPhone.wengoLoginEvent += boost::bind(&CWengoPhone::wengoLoginEventHandler, this, _1, _2, _3, _4);
	_wengoPhone.initFinishedEvent += boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.contactListCreatedEvent += boost::bind(&CWengoPhone::contactListCreatedEventHandler, this, _1, _2);
	_wengoPhone.connectHandlerCreatedEvent += boost::bind(&CWengoPhone::connectHandlerCreatedEventHandler, this, _1, _2);
}

void CWengoPhone::makeCall(const std::string & phoneNumber) {
	_wengoPhone.makeCall(phoneNumber);
}

void CWengoPhone::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	_wengoPhone.addWengoAccount(login, password, autoLogin);
}

void CWengoPhone::addContact(Contact * contact, const std::string & contactGroupName) {
	_wengoPhone.addContact(contact, contactGroupName);
}

StringList CWengoPhone::getContactGroupStringList() const {
	return _wengoPhone.getContactGroupStringList();
}

void CWengoPhone::showWengoAccount() const {
	static const string URL_WENGO_ACCOUNT = "https://www.wengo.fr/auth/auth.php";
	//TODO
	static const string langCode = "fra";

	IPhoneLine * activePhoneLine = _wengoPhone.getActivePhoneLine();
	if (activePhoneLine) {
		const SipAccount & account = activePhoneLine->getSipAccount();
		const WengoAccount & wengoAccount = dynamic_cast<const WengoAccount &>(account);
		{
			string url = URL_WENGO_ACCOUNT +
					"?login=" + wengoAccount.getWengoLogin() +
					"&password=" + wengoAccount.getWengoPassword() +
					"&lang=" + langCode +
					"&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME +
					"&page=homepage";
			WebBrowser::openUrl(url);
			LOG_DEBUG("url opened: " + url);
		}
	}
}

void CWengoPhone::start() {
	_wengoPhone.start();
	//_wengoPhone.run();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::phoneLineCreatedEventHandler(WengoPhone & sender, IPhoneLine & phoneLine) {
	static CPhoneLine cPhoneLine(phoneLine, *this);

	LOG_DEBUG("PhoneLine created");
	_pWengoPhone->addPhoneLine(cPhoneLine.getPresentation());
}

void CWengoPhone::loggerCreatedEventHandler(WengoPhone & sender, WengoPhoneLogger & logger) {
	static CWengoPhoneLogger cWengoPhoneLogger(logger, *this);

	LOG_DEBUG("WengoPhoneLogger created");
}

void CWengoPhone::contactListCreatedEventHandler(WengoPhone & sender, ContactList & contactList) {
	static CContactList cContactList(contactList, *this);

	LOG_DEBUG("ContactList created");
}

void CWengoPhone::connectHandlerCreatedEventHandler(WengoPhone & sender, ConnectHandler & connectHandler) {
	static CConnectHandler cConnectHandler(connectHandler);

	LOG_DEBUG("CConnectHandler created");
}

void CWengoPhone::wenboxPluginCreatedEventHandler(WengoPhone & sender, WenboxPlugin & wenboxPlugin) {
	static CWenboxPlugin cWenboxPlugin(wenboxPlugin, *this);

	LOG_DEBUG("WenboxPlugin created");
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::wengoLoginEventHandler(WengoPhone & sender, WengoPhone::LoginState state, const std::string & login, const std::string & password) {
	_pWengoPhone->wengoLoginStateChangedEvent(state, login, password);
}
