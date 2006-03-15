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

#include <imwrapper/IMAccount.h>

#include <imwrapper/IMAccountParser.h>

using namespace std;

IMAccount::IMAccount() {
	_protocol = EnumIMProtocol::IMProtocolUnknown;
}

IMAccount::IMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol) {
	_login = login;
	_password = password;
	_protocol = protocol;
}

IMAccount::IMAccount(const IMAccount & imAccount) {
	_login = imAccount._login;
	_password = imAccount._password;
	_protocol = imAccount._protocol;
	_settings = imAccount._settings;
}

bool IMAccount::operator == (const IMAccount & imAccount) const {
	//TODO should we check the password too?
	return ((_login == imAccount._login)
			&& (_protocol == imAccount._protocol));
}

bool IMAccount::operator < (const IMAccount & imAccount) const {
	return ((_login < imAccount._login) 
			|| ((_login == imAccount._login) && (_protocol < imAccount._protocol)));
}

string IMAccount::serialize() {
	string result;
	EnumIMProtocol enumIMProtocol;

	result += "<account protocol=\"" + enumIMProtocol.toString(_protocol) + "\">\n";
	result += ("<login>" + _login + "</login>\n");
	result += ("<password>" + _password + "</password>\n");
	result += "</account>\n";

	return result;
}

bool IMAccount::unserialize(const std::string & data) {
	IMAccountParser parser(*this, data);
	return parser.parse();
}
