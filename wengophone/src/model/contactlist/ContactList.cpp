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
#include "model/WengoPhoneLogger.h"

#include <StringList.h>

ContactList::ContactList(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {
	_dataLayer = NULL;
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
