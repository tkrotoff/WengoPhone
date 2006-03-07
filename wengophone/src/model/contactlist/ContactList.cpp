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
#include "ContactGroup.h"
#include "ContactListXMLLayer.h"
#include "ContactListParser.h"
#include "IMContactListHandler.h"

#include <model/WengoPhone.h>
#include <model/presence/PresenceHandler.h>

#include <StringList.h>
#include <Logger.h>

ContactList::ContactList(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {
	_dataLayer = NULL;

	_wengoPhone.getIMContactListHandler().newIMContactAddedEvent +=
		boost::bind(&ContactList::newIMContactAddedEventHandler, this, _1, _2, _3);
	_wengoPhone.getIMContactListHandler().imContactRemovedEvent +=
		boost::bind(&ContactList::imContactRemovedEventHandler, this, _1, _2, _3);
	_wengoPhone.getIMContactListHandler().newContactGroupAddedEvent +=
		boost::bind(&ContactList::newContactGroupAddedEventHandler, this, _1, _2);
	_wengoPhone.getIMContactListHandler().contactGroupRemovedEvent +=
		boost::bind(&ContactList::contactGroupRemovedEventHandler, this, _1, _2);
	_wengoPhone.getIMContactListHandler().imContactMovedEvent +=
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

void ContactList::addContactGroup(ContactGroup * contactGroup) {
	_contactGroupList.add(contactGroup);
	contactGroupAddedEvent(*this, *contactGroup);
	LOG_DEBUG("ContactGroup added");
}

bool ContactList::removeContactGroup(ContactGroup * contactGroup) {
	bool ret = _contactGroupList.remove(contactGroup);
	if (ret) {
		contactGroupRemovedEvent(*this, *contactGroup);
		LOG_DEBUG("ContactGroup removed");
		//delete contactGroup;
	}
	return ret;
}

ContactGroup * ContactList::operator[](unsigned int i) const {
	ContactGroup * tmp = NULL;
	try {
		tmp = _contactGroupList[i];
	} catch (OutOfRangeException & e) {
		LOG_DEBUG(e.what());
		return NULL;
	}
	return tmp;
}

ContactGroup * ContactList::operator[](const std::string & contactGroupName) const {
	for (unsigned int i = 0; i < _contactGroupList.size(); i++) {
		ContactGroup * group = this->operator[](i);
		if (group) {
			if (contactGroupName == group->toString()) {
				return group;
			}
		}
	}
	return NULL;
}

StringList ContactList::toStringList() const {
	StringList strList;
	for (unsigned int i = 0; i < _contactGroupList.size(); i++) {
		ContactGroup * group = _contactGroupList[i];
		if (group) {
			strList += group->toString();
		}
	}
	return strList;
}

std::string ContactList::serialize() {
	std::string data;

	data += "<contactList>\n";

	for (unsigned int i = 0; i < _contactGroupList.size(); i++) {
		ContactGroup * group = this->operator[](i);
		data += group->serialize();
	}

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

	ContactGroup * contactGroup = this->operator[](groupName);
	Contact * contact = NULL;

	if (!contactGroup) {
		contactGroup = new ContactGroup(groupName, _wengoPhone);
		addContactGroup(contactGroup);
	} else { 
		contact = contactGroup->findContact(newIMContact);
	}

	if (contact) {
		LOG_DEBUG("Contact found");
	} else {
		LOG_DEBUG("No Contact found, creating one");
		contact = new Contact(_wengoPhone);
		contactGroup->addContact(contact);
	}

	LOG_DEBUG("adding the IMContact");
	contact->addIMContact(newIMContact);
}

void ContactList::imContactRemovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {
	LOG_DEBUG("IMContact removed from group " + groupName + ": "
		+ imContact.getContactId());

	ContactGroup * contactGroup = this->operator[](groupName);
	Contact * contact = NULL;

	if (contactGroup) {
		contact = contactGroup->findContact(imContact);
		if (contact) {
			LOG_DEBUG("remove an IMContact from a Contact");
			contact->removeIMContact(imContact);
		}
	}
}

void ContactList::newContactGroupAddedEventHandler(IMContactList & sender,
	const std::string & groupName) {

	ContactGroup * contactGroup = this->operator[](groupName);
	if (contactGroup) {
		LOG_DEBUG("this group already exists");
	} else {
		LOG_DEBUG("new group added " + groupName);
		addContactGroup(new ContactGroup(groupName, _wengoPhone));
	}
}

void ContactList::contactGroupRemovedEventHandler(IMContactList & sender,
	const std::string & groupName) {

	ContactGroup * contactGroup = this->operator[](groupName);
	if (contactGroup) {
		LOG_DEBUG("group " + groupName + " removed");
		removeContactGroup(contactGroup);
	} else {
		LOG_DEBUG("there is no group " + groupName);
	}
}

void ContactList::presenceStateChangedEventHandler(PresenceHandler & sender, EnumPresenceState::PresenceState state, 
	const std::string & note, const IMContact & imContact) {

	ContactGroup * contactGroup;
	Contact * contact;

	for (register unsigned i = 0 ; i < size() ; i++) {
		contactGroup = this->operator[](i);
		contact = contactGroup->findContact(imContact);
		if (contact) {
			if (contact->hasIMContact(imContact)) {
				contact->getIMContact(imContact).setPresenceState(state);
				return;
			}
		}
	}

	// If ContactList is empty or no asssociated contact IMContact found
	LOG_INFO("adding a new IMContact:" + imContact.getContactId());
	contactGroup = this->operator[]("Default");
	if (!contactGroup) {
		contactGroup = new ContactGroup("Default", _wengoPhone);
		addContactGroup(contactGroup);
	}
	contact = new Contact(_wengoPhone);
	contact->addIMContact(imContact);
	contactGroup->addContact(contact);
}

void ContactList::imContactMovedEventHandler(IMContactListHandler & sender,
	const std::string & groupName, IMContact & imContact) {

	ContactGroup * oldContactGroup, * newContactGroup;
	Contact * contact;

	for (register unsigned i = 0 ; i < size() ; i++) {
		oldContactGroup = this->operator[](i);
		contact = oldContactGroup->findContact(imContact);
		if (contact) {
			newContactGroup = this->operator [](groupName);
			if (!newContactGroup) {
				newContactGroup = new ContactGroup(groupName, _wengoPhone);
				addContactGroup(newContactGroup);
			}
			newContactGroup->addContact(contact);
			oldContactGroup->removeContact(contact);
			return;
		}	
	}
}
