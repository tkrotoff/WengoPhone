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

#include "Contact.h"

#include "ContactParser.h"

#include <model/presence/PresenceHandler.h>
#include <model/imwrapper/IMAccount.h>
#include <model/imwrapper/IMContact.h>

#include <StringList.h>

#include <iostream>
using namespace std;

string Contact::serialize() {
	string result =
"<vCard>\n\
	<FN>" + getFirstName() + "</FN>\n\
	<N>\n\
		<FAMILLY>" + getLastName() + "</FAMILLY>\n\
	</N>\n";

	result = result + imContactsToString();

	result = result + "</vCard>\n";

	return result;
}

bool Contact::unserialize(const std::string & data) {
	ContactParser parser(*this, data);
	return true;
}

void Contact::initialize(const Contact & contact) {
	_firstName = contact._firstName;
	_lastName = contact._lastName;
	_sex = contact._sex;
	_birthdate = contact._birthdate;
	_picture = contact._picture;
	_website = contact._website;
	_company = contact._company;
	_wengoPhone = contact._wengoPhone;
	_mobilePhone = contact._mobilePhone;
	_homePhone = contact._homePhone;
	_workPhone = contact._workPhone;
	_otherPhone = contact._otherPhone;
	_fax = contact._fax;
	_personalEmail = contact._personalEmail;
	_workEmail = contact._workEmail;
	_otherEmail = contact._otherEmail;
	_streetAddress = contact._streetAddress;
	_notes = contact._notes;
	_state = contact._state;
	_blocked = contact._blocked;
	_imContactList = contact._imContactList;
}

Contact::~Contact() {
	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		delete (*it);
	}
}

void Contact::addIMContact(const IMContact & imContact) {
	IMContact * newContact = new IMContact(imContact);

	//Check if IMContact already exists
	if (findIMContact(_imContactList, *newContact) == _imContactList.end()) {
		subscribeToPresenceOf(*newContact);
		_imContactList.push_back(newContact);
	} else {
		delete newContact;
	}
}

void Contact::removeIMContact(const IMContact & imContact) {
	IMContactList::iterator it = findIMContact(_imContactList, imContact);

	if (it != _imContactList.end()) {
		//newContact->unsubscribeToPresence();
		_imContactList.erase(it);
	}
}

Contact::IMContactList::iterator Contact::findIMContact(IMContactList & imContactList, const IMContact & imContact) {
	IMContactList::iterator i;
	for (i = imContactList.begin() ; i != imContactList.end() ; i++) {
		if (imContact == *(*i)) {
			break;
		}
	}
	return i;
}

void Contact::block() {
	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		_presenceHandler.blockContact(*(*it));
	}
	
	_blocked = true;
}

void Contact::unblock() {
	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		_presenceHandler.unblockContact(*(*it));
	}
	
	_blocked = false;
}

string Contact::imContactsToString() {
	string result;

	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		result = result + (*it)->serialize();
	}

	return result;
}

void Contact::subscribeToPresenceOf(const IMContact & imContact) {
	_presenceHandler.subscribeToPresenceOf(imContact);
}
