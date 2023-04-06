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

#include "PhApiFileSessionManager.h"

#include "PhApiSendFileSession.h"
#include "PhApiReceiveFileSession.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <PhApiSFPCallbacks.h>
#include <PhApiSFPWrapper.h>
#include <PhApiWrapper.h>

#include <util/SafeConnect.h>

IFileSessionManagerPlugin * coip_filesession_plugin_init(CoIpManager & coIpManager) {
	return new PhApiFileSessionManager(coIpManager);
}

PhApiFileSessionManager::PhApiFileSessionManager(CoIpManager & coIpManager)
	: IFileSessionManagerPlugin(coIpManager) {

	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(newIncomingFileSignal(int, const std::string &, const std::string &, const std::string &, int)),
		SLOT(newIncomingFileSlot(int, const std::string &, const std::string &, const std::string &, int)));
	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(needsUpgradeSignal()),
		SLOT(needsUpgradeSlot()));
	SAFE_CONNECT(&PhApiSFPWrapper::getInstance(),
		SIGNAL(peerNeedsUpgradeSignal(std::string)),
		SLOT(peerNeedsUpgradeSlot(const std::string &)));
}

PhApiFileSessionManager::~PhApiFileSessionManager() {
}

ISendFileSessionPlugin * PhApiFileSessionManager::createISendFileSessionPlugin() {
	return new PhApiSendFileSession(_coIpManager);
}

void PhApiFileSessionManager::newIncomingFileSlot(int callId, const std::string & contactId,
	const std::string & fileName, const std::string & /*fileType*/, int fileSize) {

	PhApiReceiveFileSession * session =
		new PhApiReceiveFileSession(_coIpManager, callId, contactId, fileName, (unsigned) fileSize);

	newIReceiveFileSessionPluginCreatedSignal(session);
}

bool PhApiFileSessionManager::canCreateISession(const Account & account, const ContactList & contactList) const {
	bool result = false;

	EnumAccountType::AccountType accountType = account.getAccountType();
	if ((accountType == EnumAccountType::AccountTypeWengo) 
		|| (accountType == EnumAccountType::AccountTypeSIP)) {
		for (ContactList::const_iterator it = contactList.begin();
			it != contactList.end();
			it++) {
			IMContactList imContactList = (*it).getIMContactList();
			if (IMContactListHelper::hasIMContactOfProtocol(imContactList, accountType)) {
				result = true;
				break;
			}
		}
	}

	return result;	
}

IMContactList PhApiFileSessionManager::getValidIMContacts(const Account & account, const ContactList & contactList) const {
	IMContactList result;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		it++) {
		IMContactList imContactList = (*it).getIMContactList();

		// Look for an IMContact associated with the given Account
		IMContactList imContactListAsso = IMContactListHelper::getCopyOfAssociatedIMContacts(imContactList, account.getUUID());
		if (!imContactListAsso.empty()) {
			result.push_back(*imContactListAsso.begin());
			break;
		}

		// Look for an IMContact of the same protocol of the given Account
		IMContactList imContactListProto = IMContactListHelper::getCopyOfIMContactsOfProtocol(imContactList, account.getAccountType());
		if (!imContactListProto.empty()) {
			result.push_back(*imContactListProto.begin());
			break;
		}
	}

	return result;	
}

void PhApiFileSessionManager::needsUpgradeSlot() {
	needsUpgradeSignal();
}

void PhApiFileSessionManager::peerNeedsUpgradeSlot(const std::string & contactId) {
	IMContact imContact(EnumAccountType::AccountTypeWengo, contactId);
	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	ContactListHelper::getIMContact(contactList, imContact);

	peerNeedsUpgradeSignal(imContact);
}
