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

#include <imwrapper/IMContact.h>

#include <imwrapper/IMAccount.h>

#include <util/Logger.h>

using namespace std;

IMContact::IMContact() {
	_imAccount = NULL;
	_presenceState = EnumPresenceState::PresenceStateOffline;
	_protocol = EnumIMProtocol::IMProtocolUnknown;
}

IMContact::IMContact(const IMAccount & imAccount, const std::string & contactId)
	: _imAccount(&imAccount) {
	_contactId = contactId;
	_presenceState = EnumPresenceState::PresenceStateOffline;
	_protocol = EnumIMProtocol::IMProtocolUnknown;
}

IMContact::IMContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {
	_imAccount = NULL;
	_contactId = contactId;
	_presenceState = EnumPresenceState::PresenceStateOffline;
	_protocol = protocol;
}

IMContact::IMContact(const IMContact & imContact)
	: _imAccount(imContact._imAccount) {
	_contactId = imContact._contactId;
	_presenceState = imContact._presenceState;
	_protocol = imContact._protocol;
	_icon = imContact._icon;
	_alias = imContact._alias;
}

IMContact::~IMContact() {
}

bool IMContact::operator==(const IMContact & imContact) const {
	bool result = false;

	if (_imAccount && imContact._imAccount) {
		if ((*_imAccount) == (*(imContact._imAccount))) {
			if (cleanContactId() == imContact.cleanContactId()) {
				result = true;
			}
		}
	} else {
		if (getProtocol() == imContact.getProtocol()) {
			if (cleanContactId() == imContact.cleanContactId()) {
				result = true;
			}
		}
	}

	return result;
}

bool IMContact::operator<(const IMContact & imContact) const {
	bool result = false;

	if (cleanContactId() < imContact.cleanContactId()) {
		result = true;
	} else {
		if (cleanContactId() == imContact.cleanContactId()) {
			if (_imAccount && imContact._imAccount) {
				if ((*_imAccount) < (*imContact._imAccount)) {
					result = true;
				}
			} else {
				if (getProtocol() < imContact.getProtocol()) {
					result = true;
				}
			}
		}
	}

	return result;
}

string IMContact::cleanContactId() const {
	string result;
	string::size_type index = _contactId.find('/');

	if (index != string::npos) {
		result = _contactId.substr(0, index);
	} else {
		result = _contactId;
	}

	return result;
}

EnumIMProtocol::IMProtocol IMContact::getProtocol() const {
	EnumIMProtocol::IMProtocol result;

	if (_imAccount) {
		result = _imAccount->getProtocol();
	} else {
		result = _protocol;
	}

	return result;
}

void IMContact::setIMAccount(const IMAccount * imAccount) {
	if (imAccount == NULL) {
		_protocol = _imAccount->getProtocol();
	}
	_imAccount = imAccount;
}
