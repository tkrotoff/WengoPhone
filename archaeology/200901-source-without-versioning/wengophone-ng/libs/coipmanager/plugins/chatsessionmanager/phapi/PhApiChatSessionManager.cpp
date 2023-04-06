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

#include "PhApiChatSessionManager.h"

#include "PhApiChatSession.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/account/SipAccount.h>
#include <coipmanager_base/imcontact/SipIMContact.h>
#include <coipmanager_base/imcontact/WengoIMContact.h>

#include <sipwrapper/SipChatWrapper.h>
#include <PhApiWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

IChatSessionManagerPlugin * coip_chatsession_plugin_init(CoIpManager & coIpManager) {
	return new PhApiChatSessionManager(coIpManager);
}

PhApiChatSessionManager::PhApiChatSessionManager(CoIpManager & coIpManager)
	: IChatSessionManagerPlugin(coIpManager) {

	_mutex = new QMutex(QMutex::Recursive);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeWengo);

	SAFE_CONNECT(PhApiWrapper::getInstance()->getSipChatWrapper(),
		SIGNAL(chatMessageReceivedSignal(std::string, std::string, std::string)),
		SLOT(chatMessageReceivedSlot(std::string, std::string, std::string)));
	SAFE_CONNECT(PhApiWrapper::getInstance()->getSipChatWrapper(),
		SIGNAL(chatTypingStateChangedSignal(std::string, std::string, EnumChatTypingState::ChatTypingState)),
		SLOT(chatTypingStateChangedSlot(std::string, std::string, EnumChatTypingState::ChatTypingState)));
}

PhApiChatSessionManager::~PhApiChatSessionManager() {
	OWSAFE_DELETE(_mutex);
}

IChatSessionPlugin * PhApiChatSessionManager::createIChatSessionPlugin() {
	return new PhApiChatSession(_coIpManager, *this);
}


bool PhApiChatSessionManager::canCreateISession(const Account & account, const ContactList & contactList) const {
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
			} else {
				result = false;
				break;
			}
		}
	}

	return result;
}

IMContactList PhApiChatSessionManager::getValidIMContacts(const Account & account, const ContactList & contactList) const {
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

void PhApiChatSessionManager::registerChatSession(PhApiChatSession * session) {
	QMutexLocker lock(_mutex);

	_chatSessionList.push_back(session);
}

void PhApiChatSessionManager::unregisterChatSession(PhApiChatSession * session) {
	QMutexLocker lock(_mutex);

	std::remove(_chatSessionList.begin(), _chatSessionList.end(), session);
}

PhApiChatSession * PhApiChatSessionManager::retrievePhApiChatSession(const std::string & contactId) const {
	QMutexLocker lock(_mutex);

	PhApiChatSession * result = NULL;
	bool found = false;

	for (std::vector<PhApiChatSession *>::const_iterator it = _chatSessionList.begin();
		!found && (it != _chatSessionList.end());
		++it) {
		IMContactList imContactList = (*it)->getIMContactList();
		if (imContactList.size() > 0) {
			IMContact imContact = *imContactList.begin();
			SipIMContact * sipIMContact = (SipIMContact *)(imContact.getPrivateIMContact());
			// contactId should have the following form: "sip:username@realm"
			if (sipIMContact->getFullIdentity() == contactId) {
				result = *it;
				found = true;
			}
		}
	}

	return result;
}

IMContact PhApiChatSessionManager::getIMContact(const std::string & contactId) const {
	SipPeer sipPeer = SipPeer::fromFullIdentity(contactId);
	SipIMContact sipIMContact(sipPeer);
	IMContact imContact(&sipIMContact);
	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	if (!ContactListHelper::getIMContact(contactList, imContact)) {
		WengoIMContact wengoIMContact(sipPeer);
		imContact = IMContact(&wengoIMContact);
		if (!ContactListHelper::getIMContact(contactList, imContact)) {
			LOG_INFO("contact not in contactlist.");
		}
	}

	return imContact;
}

void PhApiChatSessionManager::chatMessageReceivedSlot(std::string contactIdReceiver,
	std::string contactIdSender, std::string message) {

	QMutexLocker lock(_mutex);

	LOG_DEBUG("message for=" + contactIdReceiver + " received from=" + contactIdSender + ": " + message);

	PhApiChatSession * session = retrievePhApiChatSession(contactIdSender);
	if (session) {
		// A Session with this contact already exists. Adding the message.
		session->messageReceivedSignal(getIMContact(contactIdSender), message);
	} else {
		// No Session exists. Creating one
		PhApiChatSession * session = new PhApiChatSession(_coIpManager, *this);

		// Add Contact
		IMContact imContact = getIMContact(contactIdSender);
		session->addIMContact(imContact);
		////

		// Find associated Account
		Account *foundAccount = NULL;
		UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
		for (AccountList::const_iterator it = userProfile.getAccountList().begin();
			it != userProfile.getAccountList().end();
			++it) {
			if (((*it).getAccountType() == EnumAccountType::AccountTypeSIP)
				|| ((*it).getAccountType() == EnumAccountType::AccountTypeWengo)) {
				SipAccount * sipAccount = static_cast<SipAccount *>((*it).getPrivateAccount());
				if (contactIdReceiver == sipAccount->getFullIdentity()) {
					foundAccount = (*it).clone();
				}
			}
		}

		if (foundAccount) {
			session->setAccount(*foundAccount);
			OWSAFE_DELETE(foundAccount);
		} else {
			LOG_FATAL("cannot receive a chat if no Account in database");
		}
		////

		session->start();
		newIChatSessionPluginCreatedSignal(session);
		session->messageReceivedSignal(imContact, message);
	}
}

void PhApiChatSessionManager::chatTypingStateChangedSlot(std::string contactIdReceiver,
	std::string contactIdSender, EnumChatTypingState::ChatTypingState state) {

	PhApiChatSession * session = retrievePhApiChatSession(contactIdSender);
	if (session) {
		session->typingStateChangedSignal(getIMContact(contactIdSender), state);
	} else {
		LOG_ERROR("no session found");
	}
}
