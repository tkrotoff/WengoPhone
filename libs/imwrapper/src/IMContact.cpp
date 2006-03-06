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
}

IMContact::~IMContact() {
}

bool IMContact::operator == (const IMContact & imContact) const {
	return ((_imAccount == imContact._imAccount)
		&& (_contactId == imContact._contactId));
}

bool IMContact::operator < (const IMContact & imContact) const {
	return ((_imAccount < imContact._imAccount) 
		|| (_contactId < imContact._contactId));
}

std::string IMContact::serialize() const {
	string result;
	string protocolId;

	switch (_imAccount.getProtocol()) {
	case EnumIMProtocol::IMProtocolMSN:
		protocolId = "MSNID";
		break;

	case EnumIMProtocol::IMProtocolYahoo:
		protocolId = "YAHOOID";
		break;

	case EnumIMProtocol::IMProtocolAIM:
		protocolId = "AIMID";
		break;

	case EnumIMProtocol::IMProtocolICQ:
		protocolId = "ICQID";
		break;

	case EnumIMProtocol::IMProtocolJabber:
		protocolId = "JABBERID";
		break;

	case EnumIMProtocol::IMProtocolSIPSIMPLE:
		protocolId = "WENGOID";
		break;

	default:
		LOG_FATAL("Protocol not supported.");
	}

	result = "<" + protocolId + ">"
		+ _contactId
		+ "</" + protocolId + ">\n";

	return result;
}
