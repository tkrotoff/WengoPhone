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

#include "ContactList.h"

#include "Contact.h"
#include "ContactListXMLLayer.h"
#include "ContactListParser.h"
#include "IMContactListHandler.h"

#include <model/WengoPhone.h>
#include <model/presence/PresenceHandler.h>

#include <StringList.h>
#include <Logger.h>

using namespace std;

ContactList::ContactList(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone), _imContactListHandler(wengoPhone.getIMContactListHandler()) {
	_dataLayer = NULL;

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
	_wengoPhone.getPresenceHandler().presenceStateChangedEvent +=
		boost::bind(&ContactList::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
}

ContactList::~ContactList() {
	delete _dataLayer;
}

void ContactList::load() {
	if (!_dataLayer) {
		_dataLayer = new ContactListXMLLayer(*this);
	}
}

void ContactList::addContactGroup(const string & groupName) {
	_addContactGroup(groupName);
	_imContactListHandler.addGroup(groupName);
}

void ContactList::removeContactGroup(const string & groupName) {
	_removeContactGroup(groupName);
	_imContactListHandler.removeGroup(groupName);
}

Contact & ContactList::createContact() {
	Contact contact(_wengoPhone);
	Contacts::const_iterator it;

	_contacts.push_back(contact);
	//Look for the added Contact
	for (it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if (contact == (*it)) {
			contactAddedEvent(*this, (Contact &)*it);
			break;
		}
	}

	return (Contact &)*it;
}

void ContactList::removeContact(const Contact & contact) {
	//TODO: Remove the contact from all its groups
	for (Contacts::iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if (contact == (*it)) {
			contactRemovedEvent(*this, (Contact &)*it);
			_contacts.erase(it);
		}
	}
}

void ContactList::addIMContact(Contact & contact, const IMContact & imContact) {
	contact._addIMContact(imContact);

	// We get a copy of the ContactGroupSet because the set can be modified
	// due to cascading events.
	Contact::ContactGroupSet contactGroupSet = contact._contactGroupSet;

	// Add the IMContact in all the groups of the Contact
	for (Contact::ContactGroupSet::const_iterator it = contactGroupSet.begin();
		it != contactGroupSet.end();
		++it) {
		_imContactListHandler.addIMContact(*it, imContact);
	}
}

void ContactList::removeIMContact(Contact & contact, const IMContact & imContact) {
	contact._removeIMContact(imContact);

	// We get a copy of the ContactGroupSet because the set can be modified
	// due to cascading events.
	Contact::ContactGroupSet contactGroupSet = contact._contactGroupSet;

	// Remove the IMContact in all the groups of the Contact
	for (Contact::ContactGroupSet::const_iterator it = contactGroupSet.begin();
		it != contactGroupSet.end();
		++it) {
		_imContactListHandler.removeIMContact(*it, imContact);
	}
}

void ContactList::addToContactGroup(const std::string & groupName, Contact & contact) {
	_addToContactGroup(groupName, contact);

	// We get a copy of the IMContacSet because the set can be modified
	// due to cascading events.
	IMContactSet imContactSet = contact.getIMContactSet();

	// Add all IMContact of the Contact to the group
	for (IMContactSet::const_iterator it = imContactSet.begin();
		it != imContactSet.end();
		++it) {
		_imContactListHandler.addIMContact(groupName, *it);
	}
}

void ContactList::removeFromContactGroup(const std::string & groupName, Contact & contact) {
	contact._removeFromContactGroup(groupName);

	// We get a copy of the IMContacSet because the set can be modified
	// due to cascading events.
	IMContactSet imContactSet = contact.getIMContactSet();

	// Add all IMContact of the Contact to the group
	for (IMContactSet::const_iterator it = imContactSet.begin();
		it != imContactSet.end();
		++it) {
		_imContactListHandler.removeIMContact(groupName, *it);
	}
}

std::string ContactList::serialize() {
	std::string data;

	data += "<contactList>\n";

	//TODO: full the ContactList

	data += "</contactList>\n";

	return data;
}

bool ContactList::unserialize(const std::string & data) {
	ContactListParser parser(*this, _wengoPhone, data);
	return true;
}

void ContactList::newIMContactAddedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & newIMContact) {
	LOG_DEBUG("new IMContact added in group " + groupName + ": "
		+ newIMContact.getContactId());

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(newIMContact);
	if (!contact) {
		contact = &(createContact());
	}

	contact->_addIMContact(newIMContact);
	_addToContactGroup(groupName, *contact);

	LOG_DEBUG("IMContact added");
}

void ContactList::imContactRemovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {
	LOG_DEBUG("IMContact removed from group " + groupName + ": "
		+ imContact.getContactId());

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(imContact);
	if (!contact) {
		contact = &(createContact());
	}

	contact->_removeIMContact(imContact);

	LOG_DEBUG("IMContact removed");
}

void ContactList::newContactGroupAddedEventHandler(IMContactList & sender,
	const std::string & groupName) {

	_addContactGroup(groupName);
}

void ContactList::contactGroupRemovedEventHandler(IMContactList & sender,
	const std::string & groupName) {

	_removeContactGroup(groupName);
}

void ContactList::presenceStateChangedEventHandler(PresenceHandler & sender,
	EnumPresenceState::PresenceState state,
	const std::string & note, const IMContact & imContact) {


	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(imContact);
	if (!contact) {
		LOG_INFO("adding a new IMContact:" + imContact.getContactId());
		const string groupName = "Default";
		contact = &(createContact());
		contact->_addIMContact(imContact);
		_addToContactGroup(groupName, *contact);
	}

	contact->getIMContact(imContact).setPresenceState(state);
}

void ContactList::imContactMovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {

	//TODO: implement this. an IMContact should know its groups.
}

Contact * ContactList::findContactThatOwns(const IMContact & imContact) const {
	for (Contacts::const_iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		if ((*it).hasIMContact(imContact)) {
			return (Contact *)&(*it);
		}
	}

	return NULL;
}

void ContactList::moveContactToGroup(Contact & contact, const std::string & to, const std::string & from) {
	contact.removeFromContactGroup(from);
	contact.addToContactGroup(to);
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
			((Contact &)(*vectIt))._removeFromContactGroup(groupName);
		}

		_contactGroupSet.erase(it);
		LOG_DEBUG("ContactGroup removed: " + groupName);
	} else {
		LOG_DEBUG("this ContactGroup does not exist: " + groupName);
	}	
}

void ContactList::_addToContactGroup(const std::string & groupName, Contact & contact) {
	addContactGroup(groupName);
	contact._addToContactGroup(groupName);

	ContactGroupSet::const_iterator it = _contactGroupSet.find(ContactGroup(groupName));
	if (it != _contactGroupSet.end()) {
		((ContactGroup &)(*it)).addContact(contact);
	} else {
		LOG_FATAL("the group has not been added");
	}
}

void ContactList::_removeFromContactGroup(const std::string & groupName, Contact & contact) {
	contact._removeFromContactGroup(groupName);

	((ContactGroup &)(*_contactGroupSet.find(ContactGroup(groupName)))).removeContact(contact);
}
