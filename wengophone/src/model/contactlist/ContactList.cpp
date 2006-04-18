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
	_userProfile.newIMAccountAddedEvent +=
		boost::bind(&ContactList::newIMAccountAddedEventHandler, this, _1, _2);
	_userProfile.imAccountRemovedEvent +=
		boost::bind(&ContactList::imAccountRemovedEventHandler, this, _1, _2);

}

ContactList::~ContactList() {
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
	Contact contact(_userProfile);
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
			break;
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

void ContactList::newIMContactAddedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & newIMContact) {

	LOG_DEBUG("adding a new IMContact in group " + groupName + ": " + newIMContact.getContactId());

	// Find the Contact that owns the IMContact. Creating a new one if needed
	Contact * contact = findContactThatOwns(newIMContact);
	if (!contact) {
		LOG_DEBUG("IMContact " + newIMContact.getContactId() + " not found. Adding a new Contact");
		contact = &(createContact());
	}

	if (!contact->hasIMContact(newIMContact)) {
		newIMContact.addToGroup(groupName);
		contact->_addIMContact(newIMContact);
		_addToContactGroup(groupName, *contact);

		LOG_DEBUG("IMContact added in group " + groupName + ": " + newIMContact.getContactId());
	} else {
		//This event can be received although the IMContact is already present
		// in the ContactList. We assume that this is a move event.
		imContactMovedEventHandler(sender, groupName, newIMContact);
	}
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

	LOG_DEBUG("IMContact removed: " + imContact.getContactId());
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
	const std::string & alias, const IMContact & imContact) {

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

	Contact * contact = findContactThatOwns(imContact);
	if (contact) {
		//FIXME: currently we don't know from wich group the IMContact has been
		// moved (because of Gaim limitation). So we are removing the found
		// IMContact from all its groups (although the IMContact should be in only
		// one group).
		LOG_DEBUG("IMContact moved to " + groupName);
		contact->getIMContact(imContact).removeFromAllGroup();
		contact->getIMContact(imContact).addToGroup(groupName);
	} else {
		LOG_DEBUG("IMContact not found: " + imContact.getContactId());
	}
}

Contact * ContactList::findContactThatOwns(const IMContact & imContact) const {
	for (Contacts::const_iterator it = _contacts.begin();
		it != _contacts.end();
		++it) {
		Contact & contact = (Contact &)*it;
		if (contact.hasIMContact(imContact)) {
			return &contact;
		}
	}

	return NULL;
}

void ContactList::moveContactToGroup(Contact & contact, const std::string & to, const std::string & from) {
	contact.addToContactGroup(to);
	contact.removeFromContactGroup(from);
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
		LOG_DEBUG("adding a Contact to group " + groupName);
		((ContactGroup &)(*it)).addContact(contact);
	} else {
		LOG_FATAL("the group " + groupName + " has not been added");
	}
}

void ContactList::_removeFromContactGroup(const std::string & groupName, Contact & contact) {
	if (contact.isInContactGroup(groupName)) {
		contact._removeFromContactGroup(groupName);

		LOG_DEBUG("removing a Contact from group " + groupName);
		((ContactGroup &)(*_contactGroupSet.find(ContactGroup(groupName)))).removeContact(contact);
	}
}

void ContactList::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	//TODO: link IMContact of protocol imAccount._protocol that are not linked
}

void ContactList::imAccountRemovedEventHandler(UserProfile & sender, IMAccount & imAccount) {
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
