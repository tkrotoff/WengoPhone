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
#include <imwrapper/EnumIMProtocol.h>

#include <Logger.h>

using namespace std;

IMContact::IMContact(const IMAccount & imAccount, const std::string & contactId)
	: _imAccount(imAccount) {
	_contactId = contactId;
	_presenceState = EnumPresenceState::PresenceStateOffline;
}

IMContact::IMContact(const IMContact & imContact)
	: _imAccount(imContact._imAccount) {
	_contactId = imContact._contactId;
	_presenceState = imContact._presenceState;
	_groupSet = imContact._groupSet;
}

IMContact::~IMContact() {
}

bool IMContact::operator == (const IMContact & imContact) const {
	return ((_imAccount == imContact._imAccount)
		&& (_contactId == imContact._contactId));
}

bool IMContact::operator < (const IMContact & imContact) const {
	return ((_imAccount < imContact._imAccount) 
		|| ((_imAccount == imContact._imAccount) && (_contactId < imContact._contactId)));
}

void IMContact::addToGroup(const std::string & groupName) {
	_groupSet.insert(groupName);
	imContactAddedToGroupEvent(*this, *_groupSet.find(groupName));
}

void IMContact::removeFromGroup(const std::string & groupName) {
	GroupSet::iterator it = _groupSet.find(groupName);
	if (it != _groupSet.end()) {
		imContactRemovedFromGroupEvent(*this, *it);
		_groupSet.erase(it);
	}
}

void IMContact::removeFromAllGroup() {
	for (GroupSet::const_iterator it = _groupSet.begin() ; it != _groupSet.end() ; it++) {
		imContactRemovedFromGroupEvent(*this, *it);
	}

	_groupSet.clear();
}

std::string IMContact::serialize() {
	string result;
	EnumIMProtocol enumIMProtocol;

	result += "<im protocol=\"" + enumIMProtocol.toString(_imAccount.getProtocol()) + "\">\n";

	result += ("<id>" + _contactId + "</id>");
	result += ("<account>" + _imAccount.getLogin() + "</account>");

	result += "</im>";

	return result;
}


bool IMContact::unserialize(const std::string & data) {
//	IMContactParser parser(*this, data);
//	return parser.parse();
	return false;
}

