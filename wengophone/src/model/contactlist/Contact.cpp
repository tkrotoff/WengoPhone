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

#include <model/contactlist/ContactGroup.h>
#include <model/contactlist/ContactList.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <util/Logger.h>
#include <util/Picture.h>
#include <util/StringList.h>
#include <util/Uuid.h>

#include <iostream>
using namespace std;

Contact::Contact(UserProfile & userProfile)
: ContactProfile(),
_userProfile(userProfile),
_contactList(userProfile.getContactList()) {

	profileChangedEvent +=
		boost::bind(&Contact::profileChangedEventHandler, this, _1);
}

Contact::Contact(const Contact & contact)
: _userProfile(contact._userProfile), _contactList(contact._contactList) {
	copy(contact);
}

Contact::~Contact() {
}

Contact & Contact::operator = (const Contact & contact) {
	if (&contact != this) {
		copy(contact);
	}

	contactChangedEvent(*this);

	return *this;
}

Contact & Contact::operator = (const ContactProfile & contactProfile) {
	if (&contactProfile != this) {
		copy(contactProfile);
	}

	contactChangedEvent(*this);

	return *this;
}

void Contact::copy(const Contact & contact) {
	copy((const ContactProfile &)contact);
}

void Contact::copy(const ContactProfile & contactProfile) {
	profileChangedEvent +=
		boost::bind(&Contact::profileChangedEventHandler, this, _1);

	// Sets groups
	ContactGroup * newContactGroup = _contactList.getContactGroup(contactProfile.getGroupId());
	ContactGroup * oldContactGroup = _contactList.getContactGroup(getGroupId());

	if (oldContactGroup && newContactGroup && (oldContactGroup->getUUID() != newContactGroup->getUUID())) {
		_contactList.moveContactToGroup(newContactGroup->getName(), *this);
	} else if (!oldContactGroup && newContactGroup) {
		_contactList._addToContactGroup(newContactGroup->getName(), *this);
	}
	////

	setWengoPhoneId(contactProfile._wengoPhoneId);

	Profile::copy(contactProfile);
	_uuid = contactProfile._uuid;
	_blocked = contactProfile._blocked;
	_preferredIMContact = contactProfile._preferredIMContact;
	_groupId = contactProfile._groupId;

	// Removes not present IMContacts
	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		if (!contactProfile.hasIMContact(*it)) {
			IMContactSet::const_iterator curIt = it++;
			_contactList.removeIMContact(*this, *curIt);
		}
	}
	////

	// Adds present IMContacts
	for (IMContactSet::const_iterator it = contactProfile._imContactSet.begin();
		it != contactProfile._imContactSet.end();
		++it) {
		if (!hasIMContact(*it)) {
			_contactList.addIMContact(*this, *it);
		}
	}
	////
}

bool Contact::operator == (const Contact & contact) const {
	return (ContactProfile::operator == (contact));
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
		set<IMAccount *> list =
				_userProfile.getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);
		if (list.size() > 0) {
			//Deleting previous IMContact
			IMContact oldIMContact(*(*list.begin()), _wengoPhoneId);
			removeIMContact(oldIMContact);
			////


			// Addin the new IMContact
			_wengoPhoneId = wengoId;

			IMContact newIMContact(*(*list.begin()), _wengoPhoneId);
			addIMContact(newIMContact);
			////
		}
	}
}

bool Contact::checkAndSetIMContact(const IMContact & imContact) {
	for (IMContactSet::const_iterator it = _imContactSet.begin();
		it != _imContactSet.end();
		++it) {
		if ((*it) == imContact) {
			if (!(*it).getIMAccount()) {
				((IMContact &)(*it)).setIMAccount(imContact.getIMAccount());
			}
			return true;
		}
	}

	return false;
}

void Contact::imContactChangedEventHandler(IMContact & sender) {
	contactChangedEvent(*this);
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

void Contact::profileChangedEventHandler(Profile & profile) {
	contactChangedEvent(*this);
}

void Contact::setIcon(const Picture & icon) {
}

void Contact::merge(const Contact & contact) {
	if (_firstName.empty()) {
		_firstName = contact._firstName;
	}

	if (_lastName.empty()) {
		_lastName = contact._lastName;
	}

	if (_sex == EnumSex::SexUnknown) {
		_sex = contact._sex;
	}

	//TODO: how can we detect that birthdate is not fulled
	//_birthdate = contact._birthdate;

	if (_website.empty()) {
		_website = contact._website;
	}

	if (_company.empty()) {
		_company= contact._company;
	}

	if (_wengoPhoneId.empty()) {
		_wengoPhoneId = contact._wengoPhoneId;
	}

	if (_mobilePhone.empty()) {
		_mobilePhone = contact._mobilePhone;
	}

	if (_homePhone.empty()) {
		_homePhone = contact._homePhone;
	}

	if (_workPhone.empty()) {
		_workPhone = contact._workPhone;
	}

	if (_otherPhone.empty()) {
		_otherPhone = contact._otherPhone;
	}

	if (_fax.empty()) {
		_fax = contact._fax;
	}

	if (_personalEmail.empty()) {
		_personalEmail = contact._personalEmail;
	}

	if (_workEmail.empty()) {
		_workEmail = contact._workEmail;
	}

	if (_otherEmail.empty()) {
		_otherEmail = contact._otherEmail;
	}

	// TODO: how can we detect that streetAddress is not fulled
	//_streetAddress = contact._streetAddress;

	if (_notes.empty()) {
		_notes = contact._notes;
	}

	if (!_preferredIMContact) {
		_preferredIMContact = contact._preferredIMContact;
	}

	for (IMContactSet::const_iterator it = contact._imContactSet.begin();
		it != contact._imContactSet.end();
		++it) {
		if (!hasIMContact(*it)) {
			_imContactSet.insert(*it);
		}
	}

	if (_groupId.empty()) {
			_groupId = contact._groupId;
	}
}
