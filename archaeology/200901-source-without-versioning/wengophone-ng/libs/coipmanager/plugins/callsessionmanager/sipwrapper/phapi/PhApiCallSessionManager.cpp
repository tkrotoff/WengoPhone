/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "PhApiCallSessionManager.h"

#include "PhApiCallSession.h"

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/contact/Contact.h>
#include <coipmanager_base/imcontact/IMContact.h>
#include <coipmanager_base/imcontact/IMContactList.h>

#include <PhApiFactory.h>
#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>

ICallSessionManagerPlugin * coip_callsession_plugin_init(CoIpManager & coIpManager) {
	return new PhApiCallSessionManager(coIpManager);
}

PhApiCallSessionManager::PhApiCallSessionManager(CoIpManager & coIpManager)
	: ICallSessionManagerPlugin(coIpManager) {

	_supportedAccountType.push_back(EnumAccountType::AccountTypeSIP);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeWengo);

	//Get PhApiWrapper instance
	PhApiFactory phApiFactory;
	_phApiWrapper = phApiFactory.createSipWrapper();

	//Bind to get incoming call event
	SAFE_CONNECT(_phApiWrapper, SIGNAL(phoneCallStateChangedSignal(int, EnumPhoneCallState::PhoneCallState, std::string)),
		SLOT(phoneCallStateChangedSlot(int, EnumPhoneCallState::PhoneCallState, std::string)));
}

PhApiCallSessionManager::~PhApiCallSessionManager() {
}

ICallSessionPlugin * PhApiCallSessionManager::createICallSessionPlugin() {
	return new PhApiCallSession(_coIpManager, _phApiWrapper);
}

bool PhApiCallSessionManager::canCreateISession(const Account & account, const ContactList & contactList) const {
	bool result = false;

	if ((account.getAccountType() == EnumAccountType::AccountTypeWengo)
		|| (account.getAccountType() == EnumAccountType::AccountTypeSIP)) {
		for (ContactList::const_iterator it = contactList.begin();
			it != contactList.end();
			++it) {
			IMContactList imContactList = (*it).getIMContactList();
			if (IMContactListHelper::hasIMContactOfProtocol(imContactList, EnumAccountType::AccountTypeWengo)
				|| IMContactListHelper::hasIMContactOfProtocol(imContactList, EnumAccountType::AccountTypeSIP)) {
				result = true;
				break;
			}
		}
	}

	return result;
}

IMContactList PhApiCallSessionManager::getValidIMContacts(const Account & account, const ContactList & contactList) const {
	IMContactList result;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		++it) {
		IMContactList imContactList = (*it).getIMContactList();
		for (IMContactList::const_iterator imIt = imContactList.begin();
			imIt != imContactList.end();
			++imIt) {
			EnumAccountType::AccountType accountType = (*imIt).getAccountType();
			if ((accountType == EnumAccountType::AccountTypeWengo)
				|| (accountType == EnumAccountType::AccountTypeSIP)) {
				result.push_back(*imIt);
				break;
			}
		}
	}

	return result;
}

void PhApiCallSessionManager::phoneCallStateChangedSlot(int callId,
	EnumPhoneCallState::PhoneCallState state, std::string from) {

	if (sender() != _phApiWrapper) {
		LOG_FATAL("phoneCallStateChangedSignal() not coming from our wrapper");
		return;
	}

	LOG_DEBUG("phoneCallState=" + EnumPhoneCallState::toString(state));

	if (state == EnumPhoneCallState::PhoneCallStateIncoming) {
		PhApiCallSession * session = new PhApiCallSession(_coIpManager, _phApiWrapper, callId, state, from);
		newICallSessionPluginCreatedSignal(session);
	} else {
	}
}
