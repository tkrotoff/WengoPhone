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

#include <coipmanager_base/imcontact/IMContact.h>

#include <coipmanager_base/imcontact/GenericIMContact.h>
#include <coipmanager_base/imcontact/SipIMContact.h>
#include <coipmanager_base/imcontact/WengoIMContact.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

IMContact::IMContact()
	: IIMContact() {

	_privateIMContact = NULL;
}

IMContact::IMContact(EnumAccountType::AccountType accountType, const std::string & contactId)
	: IIMContact(accountType, contactId) {

	switch (accountType) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeGTalk:
	case EnumAccountType::AccountTypeICQ:
	case EnumAccountType::AccountTypeJabber:
	case EnumAccountType::AccountTypeMSN:
	case EnumAccountType::AccountTypeIAX:
	case EnumAccountType::AccountTypeYahoo:
		_privateIMContact = new GenericIMContact(accountType, contactId);
		break;
	case EnumAccountType::AccountTypeSIP:
		_privateIMContact = new SipIMContact(contactId);
		break;
	case EnumAccountType::AccountTypeWengo:
		_privateIMContact = new WengoIMContact(contactId);
		break;
	default:
		LOG_FATAL("accountType not supported: " + accountType);
	}
}

IMContact::IMContact(const IIMContact * iIMContact)
	: IIMContact() {
	_privateIMContact = iIMContact->clone();
}

IMContact::IMContact(const IMContact & imContact)
	: IIMContact(imContact) {
	copy(imContact);
}

IMContact & IMContact::operator = (const IMContact & imContact) {
	copy(imContact);
	return *this;
}

IMContact * IMContact::clone() const {
	return new IMContact(*this);
}

void IMContact::copy(const IMContact & imContact) {
	IIMContact::copy(imContact);

	if (imContact._privateIMContact) {
		_privateIMContact = imContact._privateIMContact->clone();
	} else {
		_privateIMContact = NULL;
	}
}

IMContact::~IMContact() {
	OWSAFE_DELETE(_privateIMContact);
}

IIMContact * IMContact::getPrivateIMContact() const {
	return _privateIMContact;
}

void IMContact::setAccountId(const std::string & accountId) {
	if (_privateIMContact) {
		_privateIMContact->setAccountId(accountId);
	}
}

std::string IMContact::getAccountId() const {
	std::string result;

	if (_privateIMContact) {
		result = _privateIMContact->getAccountId();
	}

	return result;
}

void IMContact::setContactId(const std::string & contactId) {
	if (_privateIMContact) {
		_privateIMContact->setContactId(contactId);
	}
}

std::string IMContact::getContactId() const {
	std::string result;

	if (_privateIMContact) {
		result = _privateIMContact->getContactId();
	}

	return result;
}

void IMContact::setPresenceState(EnumPresenceState::PresenceState presenceState) {
	if (_privateIMContact) {
		_privateIMContact->setPresenceState(presenceState);
	}
}

EnumPresenceState::PresenceState IMContact::getPresenceState() const {
	EnumPresenceState::PresenceState result = EnumPresenceState::PresenceStateUnknown;

	if (_privateIMContact) {
		result = _privateIMContact->getPresenceState();
	}

	return result;
}

void IMContact::setPeerId(const std::string & peerId) {
	if (_privateIMContact) {
		_privateIMContact->setPeerId(peerId);
	}
}

std::string IMContact::getPeerId() const {
	std::string result;

	if (_privateIMContact) {
		result = _privateIMContact->getPeerId();
	}

	return result;
}

void IMContact::setAccountType(EnumAccountType::AccountType accountType) {
	if (_privateIMContact) {
		_privateIMContact->setAccountType(accountType);
	}
}

EnumAccountType::AccountType IMContact::getAccountType() const {
	EnumAccountType::AccountType result = EnumAccountType::AccountTypeUnknown;

	if (_privateIMContact) {
		result = _privateIMContact->getAccountType();
	}

	return result;
}

void IMContact::setAlias(const std::string & alias) {
	if (_privateIMContact) {
		_privateIMContact->setAlias(alias);
	}
}

std::string IMContact::getAlias() const {
	std::string result;

	if (_privateIMContact) {
		result = _privateIMContact->getAlias();
	}

	return result;
}

void IMContact::setIcon(const OWPicture & icon) {
	if (_privateIMContact) {
		_privateIMContact->setIcon(icon);
	}
}

OWPicture IMContact::getIcon() const {
	OWPicture result;

	if (_privateIMContact) {
		result = _privateIMContact->getIcon();
	}

	return result;
}
