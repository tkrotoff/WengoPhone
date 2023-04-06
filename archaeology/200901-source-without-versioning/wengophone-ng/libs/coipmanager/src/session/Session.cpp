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

#include <coipmanager/session/Session.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

Session::Session(CoIpManager & coIpManager)
	: CoIpModule(coIpManager) {

	_mutex = new QMutex(QMutex::Recursive);
}

Session::Session(const Session & session)
	: CoIpModule(session._coIpManager) {

	_contactList = session._contactList;
	_mutex = new QMutex(QMutex::Recursive);
}

Session::~Session() {
	OWSAFE_DELETE(_mutex);
}

EnumSessionError::SessionError Session::addContact(const Contact & contact) {
	QMutexLocker lock(_mutex);

	ContactList contactList = _contactList;
	contactList.push_back(contact);
	std::string accountId = getFirstUsableAccount(contactList);
	if (accountId.empty()) {
		return EnumSessionError::SessionErrorInvalidParameter;
	}

	LOG_DEBUG("contact added in session:" + contact.getAlias());
	_accountToUse = accountId;
	_contactList.push_back(contact);

	return EnumSessionError::SessionErrorNoError;
}

void Session::removeContact(const std::string & contact) {
	QMutexLocker lock(_mutex);

	for (ContactList::iterator it = _contactList.begin();
		it != _contactList.end();
		++it) {
		if ((*it).getUUID() == contact) {
			LOG_DEBUG("contact removed from session:" + (*it).getAlias());
			_contactList.erase(it);
		}
	}
}

EnumSessionError::SessionError Session::setContactList(const ContactList & contactList) {
	QMutexLocker lock(_mutex);

	std::string accountId = getFirstUsableAccount(contactList);
	if (accountId.empty()) {
		return EnumSessionError::SessionErrorInvalidParameter;
	}

	LOG_DEBUG("contact list of session changed");
	_accountToUse = accountId;
	_contactList = contactList;

	return EnumSessionError::SessionErrorNoError;
}

ContactList Session::getContactList() const {
	QMutexLocker lock(_mutex);

	return _contactList;
}

EnumSessionError::SessionError Session::setAccountToUse(const std::string & accountId) {
	EnumSessionError::SessionError result = EnumSessionError::SessionErrorNoError;

	//TODO: check if account is valid before setting it
	_accountToUse = accountId;

	return result;
}

void Session::contactAddedSlot(IMContact imContact) {
	//TODO: update contact list
	contactAddedSignal(getContactThatOwns(imContact));
}

void Session::contactRemovedSlot(IMContact imContact) {
	//TODO: update contact list
	contactRemovedSignal(getContactThatOwns(imContact));
}

std::string Session::getFirstUsableAccount(const ContactList & contactList) const {
	std::string result;

	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	AccountList accountList = userProfile.getAccountList();
	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		it++) {
		if (canCreateISession(*it, contactList)) {
			result = (*it).getUUID();
			break;
		}
	}

	return result;
}

Contact Session::getContactThatOwns(const IMContact & imContact) const {
	Contact result;

	// Look for imContact in Session ContactList
	Contact *contact = ContactListHelper::getCopyOfFirstContactThatOwns(_contactList, imContact);
	if (contact) {
		result = *contact;
		OWSAFE_DELETE(contact);
	} else {
		// Look for imContact in ContactList
		ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
		contact = ContactListHelper::getCopyOfFirstContactThatOwns(contactList, imContact);
		if (contact) {
			// Contact found in ContactList
			result = *contact;
			OWSAFE_DELETE(contact);
		} else {
			// Not found so we 'create' a new Contact for the Session
			result.addIMContact(imContact);
		}
	}

	return result;
}

Account * Session::getAccount() const {
	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	AccountList accountList = userProfile.getAccountList();
	return AccountListHelper::getCopyOfAccount(accountList, _accountToUse);
}

