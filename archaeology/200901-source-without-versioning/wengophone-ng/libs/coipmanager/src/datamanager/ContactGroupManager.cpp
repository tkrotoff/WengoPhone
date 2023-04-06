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

#include <coipmanager/datamanager/ContactGroupManager.h>

#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>
#include <util/Uuid.h>

ContactGroupManager::ContactGroupManager(UserProfileManager & userProfileManager, ContactGroupList & contactGroupList)
	: _userProfileManager(userProfileManager),
	_contactGroupList(contactGroupList) {

	_mutex = new QMutex(QMutex::Recursive);
}

ContactGroupManager::~ContactGroupManager() {
	OWSAFE_DELETE(_mutex);
}

std::string ContactGroupManager::add(const std::string & groupName) {
	QMutexLocker lock(_mutex);

	std::string result;

	bool found = false;
	for (ContactGroupList::const_iterator it = _contactGroupList.begin();
		it != _contactGroupList.end();
		++it) {
		if ((*it).second == groupName) {
			result = (*it).first;
			found = true;
			break;
		}
	}

	if (!found) {
		result = Uuid::generateString();
		_contactGroupList[result] = groupName;
		groupAddedSignal(result);
	}

	return result;
}

bool ContactGroupManager::remove(const std::string & groupId) {
	QMutexLocker lock(_mutex);
	bool result = false;

	ContactGroupList::iterator it = _contactGroupList.find(groupId);
	if (it != _contactGroupList.end()) {
		// Removing the group from Contacts
		ContactList contactList = _userProfileManager.getCopyOfUserProfile().getContactList();
		for (ContactList::const_iterator clIt = contactList.begin();
			clIt != contactList.end();
			++clIt) {
			removeFromGroup(groupId, (*clIt).getUUID());
		}
		////

		// Removing from group list
		_contactGroupList.erase(it);
		groupRemovedSignal(groupId);
		////

		result = true;
	}

	return result;
}

bool ContactGroupManager::rename(const std::string & groupId, const std::string & name) {
	QMutexLocker lock(_mutex);
	bool result = false;

	ContactGroupList::iterator it = _contactGroupList.find(groupId);
	if (it != _contactGroupList.end()) {
		_contactGroupList[groupId] = name;
		result = true;
	}

	return result;
}

bool ContactGroupManager::addToGroup(const std::string & groupId, const std::string & contactId) {
	QMutexLocker lock(_mutex);
	bool result = false;

	if (exists(groupId)) {
		ContactList contactList = _userProfileManager.getCopyOfUserProfile().getContactList();
		Contact * contact = ContactListHelper::getCopyOfContact(contactList, contactId);
		if (contact) {
			result = contact->addToGroup(groupId);
			_userProfileManager.getContactManager().update(*contact);
			OWSAFE_DELETE(contact);
		} else {
			LOG_ERROR("contact not found");
		}
	}

	return result;
}

bool ContactGroupManager::removeFromGroup(const std::string & groupId, const std::string & contactId) {
	QMutexLocker lock(_mutex);
	bool result = false;

	ContactList contactList = _userProfileManager.getCopyOfUserProfile().getContactList();
	Contact * contact = ContactListHelper::getCopyOfContact(contactList, contactId);
	if (contact) {
		result = contact->removeFromGroup(groupId);
		_userProfileManager.getContactManager().update(*contact);
		OWSAFE_DELETE(contact);
	} else {
		LOG_ERROR("contact not found");
	}

	return false;
}

bool ContactGroupManager::exists(const std::string & groupId) const {
	bool result = false;

	ContactGroupList::const_iterator it = _contactGroupList.find(groupId);
	if (it != _contactGroupList.end()) {
		result = true;
	}

	return result;
}
