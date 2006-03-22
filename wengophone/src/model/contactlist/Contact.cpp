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

#include <model/WengoPhone.h>
#include <model/contactlist/ContactParser.h>
#include <model/contactlist/ContactList.h>
#include <model/presence/PresenceHandler.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <iostream>
using namespace std;

Contact::Contact(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone), _contactList(wengoPhone.getContactList()) {
	_sex = SexUnknown;
	_state = EnumPresenceState::PresenceStateOffline;
	_blocked = false;
	_preferredIMContact = NULL;
}

Contact::Contact(const Contact & contact)
	: _wengoPhone(contact._wengoPhone), _contactList(contact._contactList) {
	initialize(contact);
}

Contact::~Contact() {
}

Contact & Contact::operator = (const Contact & contact) {
	if (&contact != this) {
		initialize(contact);
	}
	return *this;
}

string Contact::serialize() {
	string result =
"<vCard>\n\
	<FN>" + getFirstName() + "</FN>\n\
	<N>\n\
		<FAMILLY>" + getLastName() + "</FAMILLY>\n\
	</N>\n";

	//result = result + imContactsToString();

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
	_wengoPhoneId = contact._wengoPhoneId;
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
	_preferredIMContact = contact._preferredIMContact;
	_imContactSet = contact._imContactSet;
}

bool Contact::operator == (const Contact & contact) const {
	return 	((_firstName == contact._firstName)
		&& (_lastName == contact._lastName)
		&& (_sex == contact._sex)
		&& (_birthdate == contact._birthdate)
		&& (_picture == contact._picture)
		&& (_website == contact._website)
		&& (_company == contact._company)
		&& (_wengoPhoneId == contact._wengoPhoneId)
		&& (_mobilePhone == contact._mobilePhone)
		&& (_homePhone == contact._homePhone)
		&& (_workPhone == contact._workPhone)
		&& (_otherPhone == contact._otherPhone)
		&& (_preferredNumber == contact._preferredNumber)
		&& (_fax == contact._fax)
		&& (_personalEmail == contact._personalEmail)
		&& (_workEmail == contact._workEmail)
		&& (_otherEmail == contact._otherEmail)
		&& (_streetAddress == contact._streetAddress)
		&& (_notes == contact._notes)
		&& (_state == contact._state)
		&& (_blocked == contact._blocked)
		&& (_preferredIMContact == contact._preferredIMContact)
		&& (_imContactSet == contact._imContactSet));
}

void Contact::addIMContact(const IMContact & imContact) {
	_contactList.addIMContact(*this, imContact);
}

void Contact::removeIMContact(const IMContact & imContact) {
	_contactList.removeIMContact(*this, imContact);
}

void Contact::_addIMContact(const IMContact & imContact) {
	pair<IMContactSet::const_iterator, bool> result = _imContactSet.insert(imContact);

	if (result.second) {
		((IMContact &)(*result.first)).imContactAddedToGroupEvent +=
			boost::bind(&Contact::imContactAddedToGroupEventHandler, this, _1, _2);
		((IMContact &)(*result.first)).imContactRemovedFromGroupEvent +=
			boost::bind(&Contact::imContactRemovedFromGroupEventHandler, this, _1, _2);

		_wengoPhone.getPresenceHandler().subscribeToPresenceOf(*result.first);
		contactModifiedEvent(*this);
	}
}

void Contact::_removeIMContact(const IMContact & imContact) {
	IMContactSet::iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		contactModifiedEvent(*this);
		_imContactSet.erase(it);
	}
}

bool Contact::hasIMContact(const IMContact & imContact) const {
	if (_imContactSet.find(imContact) != _imContactSet.end()) {
		return true;
	} else {
		return false;
	}
}

IMContact & Contact::getIMContact(const IMContact & imContact) const {
	return (IMContact &)*_imContactSet.find(imContact);
}

void Contact::imContactAddedToGroupEventHandler(IMContact & sender, const std::string & groupName) {
	if (_contactGroupSet.find(groupName) == _contactGroupSet.end()) {
		_contactList._addToContactGroup(groupName, *this);
	}
}

void Contact::imContactRemovedFromGroupEventHandler(IMContact & sender, const std::string & groupName) {
//	if (_contactGroupSet.size() > 1) {
		_contactList._removeFromContactGroup(groupName, *this);
//	}
}

void Contact::addToContactGroup(const std::string & groupName) {
	_contactList.addToContactGroup(groupName, *this);
}

void Contact::removeFromContactGroup(const std::string & groupName) {
	_contactList.removeFromContactGroup(groupName, *this);
}

void Contact::_addToContactGroup(const std::string & groupName) {
	_contactGroupSet.insert(groupName);
}

void Contact::_removeFromContactGroup(const std::string & groupName) {
	ContactGroupSet::iterator it = _contactGroupSet.find(groupName);

	if (it != _contactGroupSet.end()) {
		_contactGroupSet.erase(it);
	}
}

bool Contact::isInContactGroup(const std::string & groupName) {
	ContactGroupSet::const_iterator it = _contactGroupSet.find(groupName);

	if (it != _contactGroupSet.end()) {
		return true;
	} else {
		return false;
	}
}

void Contact::block() {
	for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; it++) {
		_wengoPhone.getPresenceHandler().blockContact(*it);
	}

	_blocked = true;
}

void Contact::unblock() {
	for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; it++) {
		_wengoPhone.getPresenceHandler().unblockContact(*it);
	}

	_blocked = false;
}

bool Contact::hasIM() const {
	return (getPresenceState() != EnumPresenceState::PresenceStateOffline);
}

bool Contact::hasCall() const {
	if (!_preferredNumber.empty()
		|| wengoIsAvailable()
		|| !_mobilePhone.empty()
		|| !_homePhone.empty()) {
		return true;
	} else {
		return false;
	}
}

bool Contact::hasVideo() const {
	return wengoIsAvailable();
}

std::string Contact::getPreferredNumber() const {
	string result;

	if (!_preferredNumber.empty()) {
		result = _preferredNumber;
	} else if (wengoIsAvailable()) {
		result = _wengoPhoneId;
	} else if (!_mobilePhone.empty()) {
		result = _mobilePhone;
	} else if (!_homePhone.empty()) {
		result = _homePhone;
	} else if (!_workPhone.empty()) {
		result = _workPhone;
	} else if (!_otherPhone.empty()) {
		result = _otherPhone;
	}

	return result;
}

IMContact * Contact::getPreferredIMContact() const {
	IMContact * result = NULL;

	if (_preferredIMContact != NULL) {
		result = _preferredIMContact;
	} else {
		// Look for a connected IMContact
		for (IMContactSet::const_iterator it = _imContactSet.begin() ;
			it != _imContactSet.end() ;
			it++) {
			if ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline) {
				return (IMContact *)&(*it);
				break;
			}
		}
	}

	return result;
}

EnumPresenceState::PresenceState Contact::getPresenceState() const {
	unsigned onlineIMContact = 0;
	unsigned offlineIMContact = 0;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOffline) {
			offlineIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOnline) {
			onlineIMContact++;
		}
	}

	if (onlineIMContact > 0) {
		return EnumPresenceState::PresenceStateOnline;
	} else if (offlineIMContact == _imContactSet.size()) {
		return EnumPresenceState::PresenceStateOffline;
	} else {
		return EnumPresenceState::PresenceStateAway;
	}
}

bool Contact::wengoIsAvailable() const {
	if (!_wengoPhoneId.empty()) {
		for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; ++it) {
			if (((*it).getContactId() == _wengoPhoneId)
				&& ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline)) {
				return true;
			}
		}
	}

	return false;
}

void Contact::moveToGroup(const std::string & to, const std::string & from) {
	_contactList.moveContactToGroup(*this, to, from);
}

