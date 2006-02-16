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

#ifndef IMACCOUNT_H
#define IMACCOUNT_H

#include <imwrapper/EnumIMProtocol.h>

#include <NonCopyable.h>
#include <Settings.h>

#include <string>

/**
 * Instant Messaging account.
 *
 * Associates a login/password to an IM protocol.
 * Example: login: "bob@hotmail.com" password: "bob" protocol: IMProtocolMSN
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class IMAccount : NonCopyable {
public:

	IMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol) {
		_login = login;
		_password = password;
		_protocol = protocol;
	}

	IMAccount(const IMAccount & imAccount) {
		_login = imAccount._login;
		_password = imAccount._password;
		_protocol = imAccount._protocol;
	}

	const std::string & getLogin() const {
		return _login;
	}

	const std::string & getPassword() const {
		return _password;
	}

	EnumIMProtocol::IMProtocol getProtocol() const {
		return _protocol;
	}

	bool operator == (const IMAccount & imAccount) const {
		//TODO should we check the password too?
		return ((_login == imAccount._login)
			&& (_protocol == imAccount._protocol));
	}

	bool operator < (const IMAccount & imAccount) const {
		return ((_login < imAccount._login)
			&& (_protocol < imAccount._protocol));
	}

	Settings & getSettings() {
		return _settings;
	}

private:

	std::string _login;

	std::string _password;

	EnumIMProtocol::IMProtocol _protocol;

	/** This Settings is used to contain more parameters (eg.: "use_http => true" for MSN protocol). */
	Settings _settings;

};

#endif	//IMACCOUNT_H
