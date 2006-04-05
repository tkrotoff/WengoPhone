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

#include <model/contactlist/ContactList.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>

#include <util/StringList.h>
#include <util/Logger.h>
#include <util/Picture.h>

#include <iostream>
using namespace std;

Contact::Contact(UserProfile & userProfile)
	: _userProfile(userProfile), _contactList(userProfile.getContactList()) {
	_sex = EnumSex::SexUnknown;
	_blocked = false;
	_preferredIMContact = NULL;

	profileChangedEvent +=
		boost::bind(&Contact::profileChangedEventHandler, this, _1);
}

Contact::Contact(const Contact & contact)
	: _userProfile(contact._userProfile), _contactList(contact._contactList) {
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

void Contact::initialize(const Contact & contact) {
	_firstName = contact._firstName;
	_lastName = contact._lastName;
	_sex = contact._sex;
	_birthdate = contact._birthdate;
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
	_blocked = contact._blocked;
	_preferredIMContact = contact._preferredIMContact;
	_imContactSet = contact._imContactSet;
}

bool Contact::operator == (const Contact & contact) const {
	return ((_firstName == contact._firstName)
		&& (_lastName == contact._lastName)
		&& (_sex == contact._sex)
		&& (_birthdate == contact._birthdate)
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
		IMContact & newIMContact = (IMContact &)(*result.first);
		newIMContact.imContactAddedToGroupEvent +=
			boost::bind(&Contact::imContactAddedToGroupEventHandler, this, _1, _2);
		newIMContact.imContactRemovedFromGroupEvent +=
			boost::bind(&Contact::imContactRemovedFromGroupEventHandler, this, _1, _2);
		newIMContact.imContactChangedEvent +=
			boost::bind(&Contact::imContactChangedEventHandler, this, _1);

		_userProfile.getPresenceHandler().subscribeToPresenceOf(*result.first);
		contactChangedEvent(*this);
	}
}

void Contact::_removeIMContact(const IMContact & imContact) {
	IMContactSet::iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		contactChangedEvent(*this);
		_imContactSet.erase(it);
	}
}

void Contact::setWengoPhoneId(const string & wengoId) {
	if (!wengoId.empty()) {
		_wengoPhoneId = wengoId;

		set<IMAccount *> list;
		list = _userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);
		if (list.begin() != list.end()) {
			addIMContact(IMContact(*(*list.begin()), _wengoPhoneId));
		}
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

void Contact::imContactChangedEventHandler(IMContact & sender) {
	contactChangedEvent(*this);
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
		_userProfile.getPresenceHandler().blockContact(*it);
	}

	_blocked = true;
}

void Contact::unblock() {
	for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; it++) {
		_userProfile.getPresenceHandler().unblockContact(*it);
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
	unsigned dndIMContact = 0;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOffline) {
			offlineIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOnline) {
			onlineIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateDoNotDisturb) {
			dndIMContact++;
		}
	}

	if (onlineIMContact > 0) {
		return EnumPresenceState::PresenceStateOnline;
	} else if (dndIMContact == _imContactSet.size()) {
		return EnumPresenceState::PresenceStateDoNotDisturb;
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

void Contact::profileChangedEventHandler(Profile & profile) {
	contactChangedEvent(*this);
}

void Contact::setIcon(const Picture & icon) {
}

Picture Contact::getIcon() const {
	Picture result;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline) {
			result = _userProfile.getPresenceHandler().getContactIcon((*it));
			break;
		}
	}

	return result;
}

string Contact::getDisplayName() const {

	string result;

	if (!_firstName.empty() || !_lastName.empty()) {
		result = _firstName + " " + _lastName;
	} else {
		// Take the contact id of the first IMContact
		IMContactSet::const_iterator it = _imContactSet.begin();
		if (it != _imContactSet.end()) {
			if (!(*it).getAlias().empty()) {
				result = (*it).getAlias();
			} else {
				result = (*it).getContactId();
			}
		}
	}

	return result;
}
