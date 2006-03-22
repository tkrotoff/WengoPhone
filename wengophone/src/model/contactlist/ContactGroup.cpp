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

#include "ContactGroup.h"

#include "Contact.h"
#include "ContactGroupParser.h"
#include "IMContactListHandler.h"

#include <util/Logger.h>

ContactGroup::ContactGroup(const std::string & groupName)
	: _groupName(groupName) {
}

ContactGroup::ContactGroup(const ContactGroup & contactGroup)
	: _groupName(contactGroup._groupName), _contactList(contactGroup._contactList) {
}

void ContactGroup::addContact(Contact & contact) {
	_contactList.push_back(&contact);
	contactAddedEvent(*this, contact);
	LOG_DEBUG("Contact added to group " + _groupName);
}

void ContactGroup::removeContact(Contact & contact) {
	for (ContactVector::iterator it = _contactList.begin();
		it != _contactList.end();
		++it) {
		if ((*(*it)) == contact) {
			contactRemovedEvent(*this, *(*it));
			_contactList.erase(it);
			break;
		}
	}
}

Contact * ContactGroup::operator[](unsigned i) const {
	if ((i >= 0) || (i <= size())) {
		return _contactList[i];
	} else {
		return NULL;
	}
}

bool ContactGroup::operator == (const ContactGroup & contactGroup) const {
	return (_groupName == contactGroup._groupName);
}

bool ContactGroup::operator < (const ContactGroup & contactGroup) const {
	return (_groupName < contactGroup._groupName);
}

void ContactGroup::setName(const std::string & groupName) {
	_groupName = groupName;
	contactGroupModifiedEvent(*this);
}

