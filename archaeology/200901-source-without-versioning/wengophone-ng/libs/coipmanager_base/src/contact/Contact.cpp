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

#include <coipmanager_base/contact/Contact.h>

#include <util/Logger.h>

Contact::Contact()
	: Profile() {
}

Contact::Contact(const Contact & contact)
	: Profile(contact) {
	copy(contact);
}

Contact & Contact::operator = (const Contact & contact) {
	copy(contact);
	return *this;
}

void Contact::copy(const Contact & contact) {
	Profile::copy(contact);

	_imContactList = contact._imContactList;
	_groupList = contact._groupList;
}

Contact * Contact::clone() const {
	return new Contact(*this);
}

Contact::~Contact() {
}

bool Contact::addIMContact(const IMContact & imContact) {
	bool result = false;

	IMContactList::const_iterator it =
		std::find(_imContactList.begin(), _imContactList.end(), imContact);

	if (it == _imContactList.end()) {
		_imContactList.push_back(imContact);
		result = true;
	}

	synchronizeGroupList();

	return result;
}

bool Contact::removeIMContact(const std::string & imContactId) {
	bool result = false;

	for (IMContactList::iterator it = _imContactList.begin();
		it != _imContactList.end();
		++it) {
		if ((*it).getUUID() == imContactId) {
			_imContactList.erase(it);
			result = true;
			break;
		}
	}

	return result;
}

bool Contact::updateIMContact(const IMContact & imContact, EnumUpdateSection::UpdateSection section) {
	bool result = false;

	IMContactList::iterator it =
		std::find(_imContactList.begin(), _imContactList.end(), imContact);

	if (it == _imContactList.end()) {
		it = findByComparingIdProtocol(imContact);
	}

	if (it != _imContactList.end()) {
		switch(section) {
		case EnumUpdateSection::UpdateSectionUnknown:
			(*it) = imContact;
			break;
		case EnumUpdateSection::UpdateSectionPresenceState:
			(*it).setPresenceState(imContact.getPresenceState());
			(*it).setAlias(imContact.getAlias());
			break;
		default:
			LOG_FATAL("section not managed");
			break;
		}

		result = true;
	}

	synchronizeGroupList();

	return result;
}

const IMContactList & Contact::getIMContactList() const {
	return _imContactList;
}

EnumPresenceState::PresenceState Contact::getPresenceState() const {
	EnumPresenceState::PresenceState result = EnumPresenceState::PresenceStateUnknown;

	// Take the PresenceState of the first IMContact
	for (IMContactList::const_iterator it = _imContactList.begin();
		it != _imContactList.end();
		++it) {
		if ((*it).getPresenceState() != EnumPresenceState::PresenceStateUnknown) {
			result = (*it).getPresenceState();
			break;
		}
	}

	return result;
}

std::string Contact::getAlias() const {
	std::string result;

	// Take the alias of the first IMContact
	for (IMContactList::const_iterator it = _imContactList.begin();
		it != _imContactList.end();
		++it) {
		if (!(*it).getAlias().empty()) {
			result = (*it).getAlias();
			break;
		}
	}

	return result;
}

std::string Contact::getDisplayName() const {
	std::string result;
	std::string pseudo;
	std::string contactId;

	if (!_firstName.empty() || !_lastName.empty()) {
		result += _firstName;

		if (!result.empty() && !_lastName.empty()) {
			result += " ";
		}

		result += _lastName;
	}

	// If name set, we take the first contact id:
	if (result.empty()) {
		for (IMContactList::const_iterator it = _imContactList.begin();
			it != _imContactList.end();
			it++) {
			contactId = (*it).getContactId();
			break;
		}

		result = contactId;
	}

	pseudo = getAlias();
	if (!pseudo.empty()) {
		result += " - (" + pseudo + ")";
	}

	return result;
}

std::string Contact::getShortDisplayName() const {
	std::string toReturn = getFirstName();

	if (toReturn.empty()) {
		toReturn = getLastName();
	}

	return toReturn;
}

IMContactList::iterator Contact::findByComparingIdProtocol(const IMContact & imContact) {
	IMContactList::iterator result = _imContactList.end();

	for (IMContactList::iterator it = _imContactList.begin();
		it != _imContactList.end();
		++it) {
		if (((*it).getContactId() == imContact.getContactId())
			&& ((*it).getAccountType() == imContact.getAccountType())) {
			result = it;
			break;
		}
	}

	return result;
}

bool Contact::addToGroup(const std::string & groupId) {
	bool result = false;

	StringList::const_iterator it = 
		std::find(_groupList.begin(), _groupList.end(), groupId);
	if (it == _groupList.end()) {
		_groupList.push_back(groupId);
		result = true;
	}

	return result;
}

bool Contact::removeFromGroup(const std::string & groupId) {
	bool result = false;

	StringList::iterator it = 
		std::find(_groupList.begin(), _groupList.end(), groupId);
	if (it == _groupList.end()) {
		_groupList.erase(it);
		result = true;
	}

	return result;
}

StringList Contact::getGroupList() const {
	return _groupList;
}

void Contact::setGroupList(const StringList & stringList) {
	_groupList = stringList;
}

void Contact::synchronizeGroupList() {
	for (IMContactList::const_iterator imIt = _imContactList.begin();
		imIt != _imContactList.end();
		++imIt) {
		StringList groupList = (*imIt).getGroupList();
		for (StringList::const_iterator gIt = groupList.begin();
			gIt != groupList.end();
			++gIt) {
			StringList::const_iterator thisgit = std::find(_groupList.begin(),
				_groupList.end(), *gIt);
			if (thisgit == _groupList.end()) {
				addToGroup(*gIt);
			}
		}
	}
}
