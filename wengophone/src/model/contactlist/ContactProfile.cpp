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

#include "ContactProfile.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <util/Logger.h>
#include <util/Picture.h>
#include <util/Uuid.h>

#include <iostream>

using namespace std;

ContactProfile::ContactProfile() {
	_sex = EnumSex::SexUnknown;
	_blocked = false;
	_preferredIMContact = NULL;

	_uuid = Uuid::generate();
}

ContactProfile::ContactProfile(const ContactProfile & contactProfile) {
	copy(contactProfile);
}

ContactProfile::~ContactProfile() {
}

ContactProfile & ContactProfile::operator = (const ContactProfile & contactProfile) {
	if (&contactProfile != this) {
		copy(contactProfile);
	}

	return *this;
}

void ContactProfile::copy(const ContactProfile & contactProfile) {
	Profile::copy(contactProfile);
	_uuid = contactProfile._uuid;
	_blocked = contactProfile._blocked;
	_preferredIMContact = contactProfile._preferredIMContact;
	_imContactSet = contactProfile._imContactSet;
	_groupId = contactProfile._groupId;
}

bool ContactProfile::operator == (const ContactProfile & contactProfile) const {
	return (_uuid == contactProfile._uuid);
}

void ContactProfile::addIMContact(const IMContact & imContact) {
	pair<IMContactSet::const_iterator, bool> result = _imContactSet.insert(imContact);
}

void ContactProfile::removeIMContact(const IMContact & imContact) {
	IMContactSet::iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		_imContactSet.erase(it);
	}
}

bool ContactProfile::hasIMContact(const IMContact & imContact) const {
	if (_imContactSet.find(imContact) != _imContactSet.end()) {
		return true;
	} else {
		return false;
	}
}

IMContact & ContactProfile::getIMContact(const IMContact & imContact) const {
	return (IMContact &)*_imContactSet.find(imContact);
}

void ContactProfile::setGroupId(const std::string & groupId) {
	_groupId = groupId;
	profileChangedEvent(*this);
}

bool ContactProfile::hasIM() const {
	return (getPresenceState() != EnumPresenceState::PresenceStateOffline);
}

bool ContactProfile::hasCall() const {
	if (!getPreferredNumber().empty()) {
		return true;
	} else {
		return false;
	}
}

bool ContactProfile::hasVideo() const {
	return wengoIsAvailable();
}

std::string ContactProfile::getPreferredNumber() const {
	string result;

	if (!_preferredNumber.empty()) {
		result = _preferredNumber;
	} else if (hasFreeCall()) {
		result = getFreePhoneNumber();
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

IMContact * ContactProfile::getPreferredIMContact() const {
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

EnumPresenceState::PresenceState ContactProfile::getPresenceState() const {
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
	} else if (offlineIMContact == _imContactSet.size()) {
		return EnumPresenceState::PresenceStateOffline;
	} else if (dndIMContact == _imContactSet.size()) {
		return EnumPresenceState::PresenceStateDoNotDisturb;
	} else {
		return EnumPresenceState::PresenceStateAway;
	}
}

bool ContactProfile::wengoIsAvailable() const {
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

Picture ContactProfile::getIcon() const {
	Picture result;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if (!(*it).getIcon().getData().empty()) {
			result = (*it).getIcon();
			break;
		}
	}

	return result;
}

string ContactProfile::getDisplayName() const {
	string result;
	string pseudo;
	string contactId;

	if (!_firstName.empty() || !_lastName.empty()) {
		result += _firstName;

		if (!result.empty() && !_lastName.empty()) {
			result += " ";
		}

		result += _lastName;
	}

	// Take the alias of the first IMContact
	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		if (!(*it).getAlias().empty()) {
			pseudo = (*it).getAlias();
			contactId = (*it).getContactId();
			break;
		}
	}

	// If no alias set, we take the first contact id:
	if (contactId.empty()) {
		IMContactSet::const_iterator it = _imContactSet.begin();
		if (it != _imContactSet.end()) {
			contactId = (*it).getContactId();
		}
	}

	if (result.empty()) {
		result = contactId;
	}

	if (!pseudo.empty()) {
		result += " - (" + pseudo + ")";
	}

	return result;
}

IMContact * ContactProfile::getAvailableIMContact(IMChatSession & imChatSession) const {
	IMContact * result = NULL;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ; it != _imContactSet.end() ; ++it) {
		if (((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline)
			&& ((*(*it).getIMAccount()) == imChatSession.getIMChat().getIMAccount())) {
			result = (IMContact *)&(*it);
			break;
		}
	}

	return result;
}

std::string ContactProfile::getAvailableSIPNumber() const {
	std::string result;

	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		// If we found an IMContact that is of SIP protocol and is not
		// a Wengo ID
		if (((*it).getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE)
			&& ((*it).getContactId() != _wengoPhoneId)) {
			result = (*it).getContactId();
			break;
		}
	}

	return result;
}

bool ContactProfile::hasAvailableSIPNumber() const {
	return (!getAvailableSIPNumber().empty());
}

bool ContactProfile::hasFreeCall() const {
	return (!getFreePhoneNumber().empty());
}

std::string ContactProfile::getFreePhoneNumber() const {
	std::string result;

	if (wengoIsAvailable()) {
		result = _wengoPhoneId;
	} else if (hasAvailableSIPNumber()) {
		result = getAvailableSIPNumber();
	}

	return result;
}
