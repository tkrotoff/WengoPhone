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

#include <model/WengoPhone.h>

#include <Logger.h>

ContactGroup::ContactGroup(const std::string & groupName, WengoPhone & wengoPhone)
	:  _wengoPhone(wengoPhone) {
	_groupName = groupName;
}

void ContactGroup::addContact(Contact * contact) {
	_contactList.add(contact);
	contactAddedEvent(*this, *contact);
	LOG_DEBUG("Contact added");
}

bool ContactGroup::removeContact(Contact * contact) {
	bool ret = _contactList.remove(contact);

	if (ret) {
		contactRemovedEvent(*this, *contact);
		LOG_DEBUG("Contact removed");
	}

	return ret;
}

/*unsigned ContactGroup::size() const {
	return _contactList.size();
}*/

Contact * ContactGroup::operator[](unsigned i) const {
	Contact * tmp = NULL;
	try {
		tmp = _contactList[i];
	} catch (OutOfRangeException & e) {
		LOG_DEBUG(e.what());
		return NULL;
	}
	return tmp;
}

bool ContactGroup::operator==(const ContactGroup & contactGroup) const {
	return _groupName == contactGroup._groupName;
}

Contact * ContactGroup::findContact(const IMContact & imContact) const {
	for (register unsigned i = 0 ; i < _contactList.size() ; i++) {
		if (_contactList[i]->hasIMContact(imContact)) {
			return _contactList[i];
		}
	}

	return NULL;
}

void ContactGroup::newIMContactAddedEventHandler(Contact & sender, IMContact & imContact) {
	_wengoPhone.getIMContactListHandler().addIMContact(_groupName, imContact);
}

void ContactGroup::imContactRemovedEventHandler(Contact & sender, IMContact & imContact) {
	_wengoPhone.getIMContactListHandler().removeIMContact(_groupName, imContact);
}

std::string ContactGroup::serialize() {
	std::string data;

	data += "<contactGroup name=\"" + _groupName + "\">\n";

	for (unsigned int i = 0; i < _contactList.size(); i++) {
		Contact * contact = _contactList[i];
		data += contact->serialize();
	}

	data += "</contactGroup>\n";

	return data;
}

bool ContactGroup::unserialize(const std::string & data) {
	ContactGroupParser parser(*this, _wengoPhone, data);
	return true;
}
