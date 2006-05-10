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

#include "ContactList.h"

#include "Contact.h"
#include "IMContactListHandler.h"

#include <model/presence/PresenceHandler.h>
#include <model/profile/UserProfile.h>

#include <util/StringList.h>
#include <util/Logger.h>

using namespace std;

ContactList::ContactList(UserProfile & userProfile)
	: _userProfile(userProfile), _imContactListHandler(userProfile.getIMContactListHandler()) {

	_imContactListHandler.newIMContactAddedEvent +=
		boost::bind(&ContactList::newIMContactAddedEventHandler, this, _1, _2, _3);
	_imContactListHandler.imContactRemovedEvent +=
		boost::bind(&ContactList::imContactRemovedEventHandler, this, _1, _2, _3);
	_imContactListHandler.newContactGroupAddedEvent +=
		boost::bind(&ContactList::newContactGroupAddedEventHandler, this, _1, _2);
	_imContactListHandler.contactGroupRemovedEvent +=
		boost::bind(&ContactList::contactGroupRemovedEventHandler, this, _1, _2);
	_imContactListHandler.imContactMovedEvent +=
		boost::bind(&ContactList::imContactMovedEventHandler, this, _1, _2, _3);
	_userProfile.getPresenceHandler().presenceStateChangedEvent +=
		boost::bind(&ContactList::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
	_userProfile.getPresenceHandler().contactIconChangedEvent +=
		boost::bind(&ContactList::contactIconChangedEventHandler, this, _1, _2, _3);
	_userProfile.newIMAccountAddedEvent +=
		boost::bind(&ContactList::newIMAccountAddedEventHandler, this, _1, _2);
	_userProfile.imAccountRemovedEvent +=
		boost::bind(&ContactList::imAccountRemovedEventHandler, this, _1, _2);
}

ContactList::~ContactList() {
}

void ContactList::addContactGroup(const string & groupName) {
	//Mutex::ScopedLock lock(_mutex);

	_addContactGroup(groupName);
	_imContactListHandler.addGroup(groupName);
}

void ContactList::removeContactGroup(const string & id) {
	//Mutex::ScopedLock lock(_mutex);

	ContactGroup * contactGroup = getContactGroup(id);
	if (contactGroup) {
		_removeContactGroup(contactGroup->getName());
		_imContactListHandler.removeGroup(contactGroup->getName());
	}
}

void ContactList::renameContactGroup(const std::string & id, const std::string & name) {
	ContactGroup * contactGroup = getContactGroup(id);

	if (contactGroup) {
		std::string oldName = contactGroup->getName();
		contactGroup->setName(name);
		_imContactListHandler.changeGroupName(oldName, name);
		contactGroupRenamedEvent(*this, *contactGroup);
	}
}

Contact & ContactList::createContact() {
	Mutex::ScopedLock lock(_mutex);

	Contact contact(_userProfile);
	Contacts::const_iterator it;

	_contacts.push_back(contact);
	//Look for the added Contact
	for (it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if (contact == (*it)) {
			((Contact &) *it).contactChangedEvent +=
				boost::bind(&ContactList::contactChangedEventHandler, this, _1);
			contactAddedEvent(*this, (Contact &)*it);
			break;
		}
	}

	return (Contact &)*it;
}

void ContactList::removeContact(Contact & contact) {
	//Mutex::ScopedLock lock(_mutex);

	// Remove the Contact from its group
	ContactGroup * contactGroup = getContactGroup(contact.getGroupId());

	if (contactGroup) {
		contactGroup->removeContact(contact);
	}
	////

	// Remove all IMContacts

	// We get a copy of the IMContactSet as it will modified while browsing it
	IMContactSet imContactSet = contact.getIMContactSet();

	for (IMContactSet::const_iterator it = imContactSet.begin();
		it != imContactSet.end();
		++it) {
		removeIMContact(contact, contact.getIMContact(*it));
	}
	////

	// Remove the Contact from the ContactList
	for (Contacts::iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if (contact == (*it)) {
			contactRemovedEvent(*this, (Contact &)*it);
			_contacts.erase(it);
			break;
		}
	}
	/////
}

void ContactList::addIMContact(Contact & contact, const IMContact & imContact) {
	//Mutex::ScopedLock lock(_mutex);

	ContactGroup * contactGroup = getContactGroup(contact.getGroupId());
	
	if (contactGroup) {
		contact._addIMContact(imContact);
		_imContactListHandler.addIMContact(contactGroup->getName(), imContact);
	}
}

void ContactList::removeIMContact(Contact & contact, const IMContact & imContact) {
	//Mutex::ScopedLock lock(_mutex);

	ContactGroup * contactGroup = getContactGroup(contact.getGroupId());

	if (contactGroup) {
		_imContactListHandler.removeIMContact(contactGroup->getName(), imContact);
		contact._removeIMContact(imContact);
	}
}

void ContactList::newIMContactAddedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & newIMContact) {
	//Mutex::ScopedLock lock(_mutex);

	LOG_DEBUG("adding a new IMContact in group " + groupName + ": " + newIMContact.getContactId());

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(newIMContact);
	if (!contact) {
		LOG_DEBUG("IMContact " + newIMContact.getContactId() + " not found. Adding a new Contact");
		contact = &(createContact());
	}

	if (!contact->hasIMContact(newIMContact)) {
		_addToContactGroup(groupName, *contact);
		contact->_addIMContact(newIMContact);

		LOG_DEBUG("IMContact added in group " + groupName + ": " + newIMContact.getContactId());
	} else {
		//This event can be received although the IMContact is already present
		// in the ContactList. We assume that this is a move event.
		_moveContactToGroup(groupName, *contact);
	}
}

void ContactList::imContactRemovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {
	//Mutex::ScopedLock lock(_mutex);

	LOG_DEBUG("IMContact removed from group " + groupName + ": "
		+ imContact.getContactId());

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(imContact);
	if (!contact) {
		contact = &(createContact());
	}

	contact->_removeIMContact(imContact);

	LOG_DEBUG("IMContact removed: " + imContact.getContactId());
}

void ContactList::newContactGroupAddedEventHandler(IMContactList & sender,
	const std::string & groupName) {
	//Mutex::ScopedLock lock(_mutex);

	_addContactGroup(groupName);
}

void ContactList::contactGroupRemovedEventHandler(IMContactList & sender,
	const std::string & groupName) {
	//Mutex::ScopedLock lock(_mutex);

	_removeContactGroup(groupName);
}

void ContactList::presenceStateChangedEventHandler(PresenceHandler & sender,
	EnumPresenceState::PresenceState state,
	const std::string & alias, const IMContact & imContact) {
	//Mutex::ScopedLock lock(_mutex);

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(imContact);
	if (!contact) {
		LOG_INFO("adding a new IMContact:" + imContact.getContactId());
		const string groupName = "Default";
		contact = &(createContact());
		contact->_addIMContact(imContact);
		_addToContactGroup(groupName, *contact);
	}

	// The PresenceState must not be changed if the PresenceState is
	// UserDefined (used by PhApi to set the alias)
	if (state != EnumPresenceState::PresenceStateUserDefined) {
		contact->getIMContact(imContact).setPresenceState(state);
		if (imContact.getIMAccount()->getProtocol() != EnumIMProtocol::IMProtocolSIPSIMPLE) {
			contact->getIMContact(imContact).setAlias(alias);
		}
	} else {
		contact->getIMContact(imContact).setPresenceState(EnumPresenceState::PresenceStateOnline);
		contact->getIMContact(imContact).setAlias(alias);
		//contact->getIMContact(imContact).setIcon();
	}
}

void ContactList::imContactMovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {
	//Mutex::ScopedLock lock(_mutex);

	Contact * contact = findContactThatOwns(imContact);
	if (contact) {
		// This method can be called when a Contact has changed, not only
		// because the Contact has moved.
		ContactGroup * contactGroup = getContactGroup(contact->getGroupId());
		if (contactGroup && (contactGroup->getName() == groupName)) {
			contactChangedEvent(*this, *contact);
		} else {
			_moveContactToGroup(groupName, *contact);
		}
	}
}

Contact * ContactList::findContactThatOwns(const IMContact & imContact) const {
	Contact * result = NULL;

	for (Contacts::const_iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		Contact & contact = (Contact &)*it;
		if (contact.checkAndSetIMContact(imContact)) {
			result = &contact;
			break;
		}
	}

	return result;
}

void ContactList::_addContactGroup(const std::string & groupName) {
	ContactGroup contactGroup(groupName);
	pair<ContactGroupSet::const_iterator, bool> result = _contactGroupSet.insert(contactGroup);

	//If the ContactGroup was actually inserted (e.g did not exist)
	if (result.second) {
		contactGroupAddedEvent(*this, (ContactGroup &)*result.first);
		LOG_DEBUG("new ContactGroup added " + groupName);
	} else {
		LOG_DEBUG("this ContactGroup already exists: " + groupName);
	}
}

void ContactList::_removeContactGroup(const std::string & groupName) {
	ContactGroup contactGroup(groupName);
	ContactGroupSet::iterator it = _contactGroupSet.find(groupName);

	if (it != _contactGroupSet.end()) {
		for (Contacts::const_iterator vectIt = _contacts.begin();
			vectIt != _contacts.end();
			++vectIt) {
			removeContact((Contact &)*vectIt);
		}

		_contactGroupSet.erase(it);
		LOG_DEBUG("ContactGroup removed: " + groupName);
	} else {
		LOG_DEBUG("this ContactGroup does not exist: " + groupName);
	}
}

void ContactList::_addToContactGroup(const std::string & groupName, Contact & contact) {
	if (!groupName.empty()) {
		addContactGroup(groupName);

		ContactGroupSet::const_iterator it = _contactGroupSet.find(ContactGroup(groupName));
		if (it != _contactGroupSet.end()) {
			LOG_DEBUG("adding a Contact to group " + groupName);
			((ContactGroup &)(*it)).addContact(contact);
			contact.setGroupId((*it).getUUID());
		} else {
			LOG_FATAL("the group " + groupName + " has not been added");
		}
	}
}

void ContactList::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	//Mutex::ScopedLock lock(_mutex);
}

void ContactList::imAccountRemovedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	//Mutex::ScopedLock lock(_mutex);

	for (Contacts::const_iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		for (IMContactSet::const_iterator imContactIt = ((Contact &)*it).getIMContactSet().begin();
			imContactIt != ((Contact &)*it).getIMContactSet().end();
			++imContactIt) {
			if ((*imContactIt).getIMAccount() &&
					((*(*imContactIt).getIMAccount()) == imAccount)) {
				((IMContact &)*imContactIt).setIMAccount(NULL);
			}
		}
	}
}

ContactGroup * ContactList::getContactGroup(const std::string & groupId) const {
	ContactGroup * result = NULL;

	for (ContactGroupSet::const_iterator it = _contactGroupSet.begin();
		it != _contactGroupSet.end();
		++it) {
		if ((*it).getUUID() == groupId) {
			result = &(ContactGroup &)(*it);
			break;
		}
	}

	return result;
}

void ContactList::contactIconChangedEventHandler(PresenceHandler & sender,
	const IMContact & imContact, Picture icon) {
	//Mutex::ScopedLock lock(_mutex);

	Contact * contact = findContactThatOwns(imContact);
	if (contact) {
		contact->getIMContact(imContact).setIcon(icon);
	} else {
		LOG_DEBUG("IMContact not found: " + imContact.getContactId());
	}
}

void ContactList::mergeContacts(Contact & dst, Contact & src) {
	//Mutex::ScopedLock lock(_mutex);

	dst.merge(src);

	// Remove the source Contact without removing it from linked IMContactLists
	// Remove the Contact from its group
	ContactGroup * contactGroup = getContactGroup(src.getGroupId());
	if (contactGroup) {
		contactGroup->removeContact(src);
	}
	////

	for (Contacts::iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if (src == (*it)) {
			contactRemovedEvent(*this, (Contact &)*it);
			_contacts.erase(it);
			break;
		}
	}
	////
}

void ContactList::_moveContactToGroup(const string & dst, Contact & contact) {
	Mutex::ScopedLock lock(_mutex);

	ContactGroup * contactGroup = getContactGroup(contact.getGroupId());
	std::string oldGroupName;

	if (contactGroup) {
		oldGroupName = contactGroup->getName();
	}

	if (oldGroupName != dst) {
		addContactGroup(dst);

		ContactGroupSet::iterator oldIt = _contactGroupSet.find(ContactGroup(oldGroupName));
		ContactGroupSet::iterator newIt = _contactGroupSet.find(ContactGroup(dst));

		if ((newIt != _contactGroupSet.end())
			&& (oldIt != _contactGroupSet.end())) {
			((ContactGroup &)(*oldIt)).removeContact(contact);
			((ContactGroup &)(*newIt)).addContact(contact);

			contact.setGroupId((*newIt).getUUID());
			contactMovedEvent(*this, (ContactGroup &)*newIt, (ContactGroup &)*oldIt, contact);
		}
	}
}

void ContactList::moveContactToGroup(const string & dst, Contact & contact) {
	//Mutex::ScopedLock lock(_mutex);

	ContactGroup * contactGroup = getContactGroup(contact.getGroupId());
	std::string oldGroupName;

	if (contactGroup) {
		oldGroupName = contactGroup->getName();
	}

	if (oldGroupName != dst) {
		addContactGroup(dst);

		ContactGroupSet::iterator oldIt = _contactGroupSet.find(ContactGroup(oldGroupName));
		ContactGroupSet::iterator newIt = _contactGroupSet.find(ContactGroup(dst));

		for (IMContactSet::const_iterator it = contact.getIMContactSet().begin();
			it != contact.getIMContactSet().end();
			++it) {
			_imContactListHandler.moveContactToGroup((*newIt).getName(), oldGroupName, (IMContact &)*it);
		}

		if ((newIt != _contactGroupSet.end())
			&& (oldIt != _contactGroupSet.end())) {
			((ContactGroup &)(*oldIt)).removeContact(contact);
			((ContactGroup &)(*newIt)).addContact(contact);

			contact.setGroupId((*newIt).getUUID());
			contactMovedEvent(*this, (ContactGroup &)*newIt, (ContactGroup &)*oldIt, contact);
		}
	}
}

Contact * ContactList::getContact(const std::string & contactId) const {
	for (Contacts::const_iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		LOG_DEBUG("Looking for: " + contactId + ", current: " + (*it).getUUID());
		if ((*it).getUUID() == contactId) {
			return &(Contact &)(*it);
			break;
		}
	}

	return NULL;
}

void ContactList::contactChangedEventHandler(Contact & sender) {
	contactChangedEvent(*this, sender);
}
