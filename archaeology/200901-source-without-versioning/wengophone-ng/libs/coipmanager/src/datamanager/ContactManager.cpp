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

#include <coipmanager/datamanager/ContactManager.h>

#include <coipmanager_base/contact/Contact.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

ContactManager::ContactManager(ContactList & contactList)
	: _contactList(contactList) {

	_mutex = new QMutex(QMutex::Recursive);
}

ContactManager::~ContactManager() {
	OWSAFE_DELETE(_mutex);
}

bool ContactManager::add(const Contact & contact) {
	QMutexLocker lock(_mutex);

	bool result = false;

	ContactList::iterator it = findContact(_contactList, contact);
	if (it == _contactList.end()) {
		_contactList.push_back(contact);
		contactAddedSignal(contact.getUUID());
		// Emitting events for IMContacts
		IMContactList list = contact.getIMContactList();
		for (IMContactList::const_iterator imIt = list.begin();
			imIt != list.end();
			++imIt) {
			imContactAddedSignal(contact.getUUID(), *imIt);
		}
		////
		result = true;
	}

	return result;
}

bool ContactManager::remove(const std::string & contactId) {
	QMutexLocker lock(_mutex);

	bool result = false;

	for (ContactList::iterator it = _contactList.begin();
		it != _contactList.end();
		it++) {
		if ((*it).getUUID() == contactId) {
			// Emitting events for IMContacts
			IMContactList list = (*it).getIMContactList();
			for (IMContactList::const_iterator imIt = list.begin();
				imIt != list.end();
				++imIt) {
				imContactRemovedSignal((*it).getUUID(), *imIt);
			}
			////
			_contactList.erase(it);
			contactRemovedSignal(contactId);
			result = true;
			break;
		}
	}

	return result;
}

bool ContactManager::update(const Contact & contact, EnumUpdateSection::UpdateSection section) {
	QMutexLocker lock(_mutex);

	bool result = false;

	ContactList::iterator it = findContact(_contactList, contact);
	if (it != _contactList.end()) {
		IMContactList newList = contact.getIMContactList();
		IMContactList oldList = (*it).getIMContactList();

		// Getting added IMContact
		for (IMContactList::const_iterator imIt = newList.begin();
			imIt != newList.end();
			++imIt) {
			if (!IMContactListHelper::has(oldList, *imIt)) {
				imContactAddedSignal((*it).getUUID(), *imIt);
			}
		}
		////

		// Getting removed IMContact
		for (IMContactList::const_iterator imIt = oldList.begin();
			imIt != oldList.end();
			++imIt) {
			if (!IMContactListHelper::has(newList, *imIt)) {
				imContactRemovedSignal((*it).getUUID(), *imIt);
			}
		}
		////

		(*it) = contact;
		contactUpdatedSignal(contact.getUUID(), section);
		result = true;
	}

	return result;
}

ContactList::iterator ContactManager::findContact(ContactList & contactList, const Contact & contact) {
	ContactList::iterator result;

	for (result = contactList.begin(); result != contactList.end(); ++result) {
		if ((*result) == contact) {
			break;
		}
	}

	return result;
}
