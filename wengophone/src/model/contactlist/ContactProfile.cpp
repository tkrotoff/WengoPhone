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

#include "ContactPresenceStateUnknown.h"
#include "ContactPresenceStateOnline.h"
#include "ContactPresenceStateOffline.h"
#include "ContactPresenceStateAway.h"
#include "ContactPresenceStateDoNotDisturb.h"
#include "ContactPresenceStateUnavailable.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <util/Logger.h>
#include <util/OWPicture.h>
#include <util/Uuid.h>

#include <iostream>

using namespace std;

ContactProfile::PresenceStates ContactProfile::_presenceStateMap;

static ContactPresenceStateUnknown contactPresenceStateUnknown;
static ContactPresenceStateOnline contactPresenceStateOnline;
static ContactPresenceStateOffline contactPresenceStateOffline;
static ContactPresenceStateAway contactPresenceStateAway;
static ContactPresenceStateDoNotDisturb contactPresenceStateDoNotDisturb;
static ContactPresenceStateUnavailable contactPresenceStateUnavailable;

ContactProfile::ContactProfile() {
	_sex = EnumSex::SexUnknown;
	_blocked = false;
	_preferredIMContact = NULL;

	_uuid = Uuid::generateString();

	//Default state (ContactPresenceStateUnknown)
	_presenceState = &contactPresenceStateUnknown;

	_presenceStateMap[contactPresenceStateUnknown.getCode()] = &contactPresenceStateUnknown;
	_presenceStateMap[contactPresenceStateOnline.getCode()] = &contactPresenceStateOnline;
	_presenceStateMap[contactPresenceStateOffline.getCode()] = &contactPresenceStateOffline;
	_presenceStateMap[contactPresenceStateAway.getCode()] = &contactPresenceStateAway;
	_presenceStateMap[contactPresenceStateDoNotDisturb.getCode()] = &contactPresenceStateDoNotDisturb;
	_presenceStateMap[contactPresenceStateUnavailable.getCode()] = &contactPresenceStateUnavailable;
}

ContactProfile::ContactProfile(const ContactProfile & contactProfile) {
	copy(contactProfile);
}

ContactProfile::~ContactProfile() {
}

ContactProfile & ContactProfile::operator=(const ContactProfile & contactProfile) {
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
	_presenceState = contactProfile._presenceState;
}

bool ContactProfile::operator == (const ContactProfile & contactProfile) const {
	return (_uuid == contactProfile._uuid);
}

void ContactProfile::addIMContact(const IMContact & imContact) {
	pair<IMContactSet::const_iterator, bool> result = _imContactSet.insert(imContact);
	updatePresenceState();
}

void ContactProfile::removeIMContact(const IMContact & imContact) {
	IMContactSet::iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		_imContactSet.erase(it);
	}
	updatePresenceState();
}

bool ContactProfile::hasIMContact(const IMContact & imContact) const {
	if (_imContactSet.find(imContact) != _imContactSet.end()) {
		return true;
	} else {
		return false;
	}
}

IMContact * ContactProfile::getIMContact(const IMContact & imContact) const {
	IMContactSet::const_iterator it = _imContactSet.find(imContact);
	if (it != _imContactSet.end()) {
		return ((IMContact *) &(*it));
	} else {
		return NULL;
	}
}

void ContactProfile::setGroupId(const std::string & groupId) {
	_groupId = groupId;
	profileChangedEvent(*this);
}

bool ContactProfile::hasIM() const {
	return (getPresenceState() != EnumPresenceState::PresenceStateOffline &&
		getPresenceState() != EnumPresenceState::PresenceStateUnknown);
}

bool ContactProfile::hasCall() const {
	if (!getPreferredNumber().empty()) {
		return true;
	} else {
		return false;
	}
}

bool ContactProfile::hasVideo() const {
	return hasAvailableWengoId();
}

std::string ContactProfile::getPreferredNumber() const {
	string result;

	if (!_preferredNumber.empty()) {
		result = _preferredNumber;
	} else if (hasFreeCall()) {
		result = getFirstFreePhoneNumber();
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
	return _presenceState->getCode();
}

EnumPresenceState::PresenceState ContactProfile::computePresenceState() const {
	unsigned onlineIMContact = 0;
	unsigned offlineIMContact = 0;
	unsigned dndIMContact = 0;
	unsigned awayIMContact = 0;

	for (IMContactSet::const_iterator it = _imContactSet.begin() ;
		it != _imContactSet.end() ;
		it++) {
		if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOffline) {
			offlineIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateOnline) {
			onlineIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateDoNotDisturb) {
			dndIMContact++;
		} else if ((*it).getPresenceState() == EnumPresenceState::PresenceStateAway) {
			awayIMContact++;
		}
	}

	if (onlineIMContact > 0) {
		return EnumPresenceState::PresenceStateOnline;
	} else if ( (!hasPhoneNumber()) && (_imContactSet.size() != 0) && (offlineIMContact == _imContactSet.size())) {
		return EnumPresenceState::PresenceStateOffline;
	} else if ((_imContactSet.size() != 0) && (dndIMContact == _imContactSet.size())) {
		return EnumPresenceState::PresenceStateDoNotDisturb;
	} else if (awayIMContact > 0) {
		return EnumPresenceState::PresenceStateAway;
	} else if (hasPhoneNumber() && !hasAvailableWengoId()) {
		return EnumPresenceState::PresenceStateUnavailable;
	} else {
		return EnumPresenceState::PresenceStateUnknown;
	}
}

void ContactProfile::updatePresenceState() {
	EnumPresenceState::PresenceState presenceState = computePresenceState();

	LOG_DEBUG("PresenceState=" + String::fromNumber(presenceState));

	PresenceStates::iterator it = _presenceStateMap.find(presenceState);
	if (it == _presenceStateMap.end()) {
		LOG_FATAL("unknown PresenceState=" + String::fromNumber(presenceState));
	}

	ContactPresenceState * state = it->second;
	if (state->getCode() == presenceState) {
		if (_presenceState->getCode() != state->getCode()) {
			_presenceState = state;
			_presenceState->execute(*this);
			LOG_DEBUG("presence state changed=" + EnumPresenceState::toString(_presenceState->getCode()));
			profileChangedEvent(*this);
			return;
		}
	}
}

OWPicture ContactProfile::getIcon() const {
	OWPicture result;

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

std::string ContactProfile::getShortDisplayName() const {
	std::string toReturn = getFirstName();
	if (toReturn.empty()) {
		toReturn = getLastName();
	}

	return std::string(toReturn);
}

IMContact * ContactProfile::getFirstAvailableIMContact(IMChatSession & imChatSession) const {
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

std::string ContactProfile::getFirstAvailableSIPNumber() const {
	std::string result;

	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		if ((*it).getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
			result = (*it).getContactId();
			break;
		}
	}

	return result;
}

bool ContactProfile::hasAvailableSIPNumber() const {
	return (!getFirstAvailableSIPNumber().empty());
}

std::string ContactProfile::getFirstAvailableWengoId() const {
	std::string result;

	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
			
		if (((*it).getProtocol() == EnumIMProtocol::IMProtocolWengo)
			&& ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline)) {
			result = (*it).getContactId();
			break;
		}
	}

	return result;
}

std::string ContactProfile::getFirstWengoId() const {
	std::string result;

	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		if (((*it).getProtocol() == EnumIMProtocol::IMProtocolWengo)) {
			result = (*it).getContactId();
			break;
		}
	}

	return result;
}

bool ContactProfile::hasAvailableWengoId() const {
	return (!getFirstAvailableWengoId().empty());
}

std::string ContactProfile::getFirstFreePhoneNumber() const {
	std::string result;

	if (hasAvailableWengoId()) {
		result = getFirstAvailableWengoId();
	} else if (hasAvailableSIPNumber()) {
		result = getFirstAvailableSIPNumber();
	}

	return result;
}

bool ContactProfile::hasFreeCall() const {
	return (!getFirstFreePhoneNumber().empty());
}

void ContactProfile::setWengoPhoneId(const std::string & wengoPhoneId) {
	Profile::setWengoPhoneId(wengoPhoneId);
	addIMContact(IMContact(EnumIMProtocol::IMProtocolWengo, wengoPhoneId));
}

bool ContactProfile::isAvailable() const {
	return ((getPresenceState() != EnumPresenceState::PresenceStateOffline)
		&& ((getPresenceState() != EnumPresenceState::PresenceStateUnknown)));
}
