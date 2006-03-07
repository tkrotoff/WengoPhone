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

#include <model/WengoPhone.h>
#include <model/presence/PresenceHandler.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>

#include <StringList.h>
#include <Logger.h>

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
	_imContactSet = contact._imContactSet;
}

Contact::~Contact() {
}

bool Contact::operator == (const Contact & contact) {
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
		&& (_fax == contact._fax)
		&& (_personalEmail == contact._personalEmail)
		&& (_workEmail == contact._workEmail)
		&& (_otherEmail == contact._otherEmail)
		&& (_streetAddress == contact._streetAddress)
		&& (_notes == contact._notes)
		&& (_state == contact._state)
		&& (_blocked == contact._blocked)
		&& (_imContactSet == contact._imContactSet));
}

void Contact::addIMContact(const IMContact & imContact) {
	//Check if IMContact already exists
	if (_imContactSet.find(imContact) == _imContactSet.end()) {
		_imContactSet.insert(imContact);
		_wengoPhone.getPresenceHandler().subscribeToPresenceOf(*_imContactSet.find(imContact));
		newIMContactAddedEvent(*this, (IMContact &)*_imContactSet.find(imContact));
		contactModifiedEvent(*this);
	}
}

void Contact::removeIMContact(const IMContact & imContact) {
	IMContactSet::iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		imContactRemovedEvent(*this, (IMContact &)*it);
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

string Contact::imContactsToString() {
	string result;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; it++) {
		result = result + (*it).serialize();
	}

	return result;
}

bool Contact::haveIM() const {
	return (getPresenceState() != EnumPresenceState::PresenceStateOffline);
}

bool Contact::haveCall() const {
	if (wengoIsAvailable()
		|| !_mobilePhone.empty()
		|| !_homePhone.empty()) {
		return true;
	} else {
		return false;
	}
}

bool Contact::haveVideo() const {
	//FIXME: can we check if we can place a video call
	return wengoIsAvailable();
}

void Contact::placeCall() {
	if (wengoIsAvailable()) {
		_wengoPhone.makeCall(_wengoPhoneId);
	} else if (!_mobilePhone.empty()) {
		_wengoPhone.makeCall(_mobilePhone);
	} else if (!_homePhone.empty()) {
		_wengoPhone.makeCall(_homePhone);
	}
}

void Contact::placeVideoCall() {
	if (wengoIsAvailable()) {
		_wengoPhone.makeCall(_wengoPhoneId);
	}
}

void Contact::startIM() {
	LOG_DEBUG("start a chat with " + _firstName);
	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline) {
			IMContactSet list;
			list.insert((*it));
			_wengoPhone.getChatHandler().createSession((*it).getIMAccount(), list);
			break;
		}
	}
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
