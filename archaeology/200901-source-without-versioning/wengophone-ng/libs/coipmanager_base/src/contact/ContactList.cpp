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

#include <coipmanager_base/contact/ContactList.h>

#include <util/SafeDelete.h>

Contact * ContactListHelper::getCopyOfContact(const ContactList & contactList, const std::string & uuid) {
	Contact * result = NULL;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		++it) {
		if ((*it).getUUID() == uuid) {
			result = (*it).clone();
			break;
		}
	}

	return result;
}

Contact * ContactListHelper::getCopyOfFirstContactThatOwns(const ContactList & contactList, const std::string & imContactId) {
	Contact * result = NULL;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		++it) {
		if (IMContactListHelper::has((*it).getIMContactList(), imContactId)) {
			result = (*it).clone();
			break;
		}
	}

	return result;
}

Contact * ContactListHelper::getCopyOfFirstContactThatOwns(const ContactList & contactList, const IMContact & imContact) {
	Contact * result = NULL;

	for (ContactList::const_iterator it = contactList.begin();
		it != contactList.end();
		++it) {
		if (IMContactListHelper::has((*it).getIMContactList(), imContact)) {
			result = (*it).clone();
			break;
		}
	}

	return result;
}

bool ContactListHelper::getIMContact(const ContactList & contactList, IMContact & imContact) {
	bool result = false;

	Contact * contact = getCopyOfFirstContactThatOwns(contactList, imContact);
	if (contact) {
		IMContact *foundIMContact = IMContactListHelper::getCopyOfIMContact(contact->getIMContactList(), imContact);
		if (foundIMContact) {
			imContact = *foundIMContact;
			result = true;
			OWSAFE_DELETE(foundIMContact);
		}
		OWSAFE_DELETE(contact);
	}

	return result;
}
