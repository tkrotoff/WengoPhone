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

#include <coipmanager_base/account/OscarAccount.h>

#include <util/Logger.h>
#include <util/String.h>

const std::string OscarAccount::DEFAULT_OSCAR_SERVER = "login.oscar.aol.com";
const int OscarAccount::DEFAULT_OSCAR_PORT = 5190;
const std::string OscarAccount::DEFAULT_ENCODING = "ISO-8859-1";

OscarAccount::OscarAccount()
	: IAccount(String::null, String::null, EnumAccountType::AccountTypeUnknown) {
	_oscarServer = DEFAULT_OSCAR_SERVER;
	_oscarServerPort = DEFAULT_OSCAR_PORT;
	_oscarEncoding = DEFAULT_ENCODING;
}

OscarAccount::OscarAccount(const std::string & login, const std::string & password)
	: IAccount(login, password, EnumAccountType::AccountTypeUnknown) {
	_oscarServer = DEFAULT_OSCAR_SERVER;
	_oscarServerPort = DEFAULT_OSCAR_PORT;
	_oscarEncoding = DEFAULT_ENCODING;
}

OscarAccount::OscarAccount(const OscarAccount & account)
	: IAccount(account) {
	copy(account);
}

OscarAccount::OscarAccount(const IAccount * iAccount) {
	const OscarAccount * account = dynamic_cast<const OscarAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an OscarAccount");
	}
}

OscarAccount::~OscarAccount() {
}

OscarAccount * OscarAccount::clone() const {
	return new OscarAccount(*this);
}

void OscarAccount::copy(const OscarAccount & account) {
	IAccount::copy(account);
	_oscarServer = account._oscarServer;
	_oscarServerPort = account._oscarServerPort;
	_oscarEncoding = account._oscarEncoding;
}

void OscarAccount::setServer(const std::string & oscarServer) {
	_oscarServer = oscarServer;
}

std::string OscarAccount::getServer() const {
	return _oscarServer;
}

void OscarAccount::setServerPort(int oscarServerPort) {
	_oscarServerPort = oscarServerPort;
}

int OscarAccount::getServerPort() const {
	return _oscarServerPort;
}

void OscarAccount::setEncoding(const std::string & encoding) {
	_oscarEncoding = encoding;
}

std::string OscarAccount::getEncoding() const {
	return _oscarEncoding;
}
