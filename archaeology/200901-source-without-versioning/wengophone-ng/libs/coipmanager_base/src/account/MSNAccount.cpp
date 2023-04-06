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

#include <coipmanager_base/account/MSNAccount.h>

#include <util/Logger.h>
#include <util/String.h>

const std::string MSNAccount::DEFAULT_MSN_SERVER = "messenger.hotmail.com";
const int MSNAccount::DEFAULT_MSN_PORT = 1863;

MSNAccount::MSNAccount()
	: IAccount(String::null, String::null, EnumAccountType::AccountTypeMSN) {
	_msnServer = DEFAULT_MSN_SERVER;
	_msnServerPort = DEFAULT_MSN_PORT;
	_useHttp = false;
	_mailNotified = false;
}

MSNAccount::MSNAccount(const std::string & login, const std::string & password)
	: IAccount(login, password, EnumAccountType::AccountTypeMSN) {
	_msnServer = DEFAULT_MSN_SERVER;
	_msnServerPort = DEFAULT_MSN_PORT;
	_useHttp = false;
	_mailNotified = false;
}

MSNAccount::MSNAccount(const MSNAccount & account)
	: IAccount(account) {
	copy(account);
}

MSNAccount::MSNAccount(const IAccount * iAccount) {
	const MSNAccount * account = dynamic_cast<const MSNAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an MSNAccount");
	}
}

MSNAccount::~MSNAccount() {
}

MSNAccount * MSNAccount::clone() const {
	return new MSNAccount(*this);
}

void MSNAccount::copy(const MSNAccount & account) {
	IAccount::copy(account);
	_msnServer = account._msnServer;
	_msnServerPort = account._msnServerPort;
	_useHttp = account._useHttp;
	_mailNotified = account._mailNotified;
}

void MSNAccount::setMailNotification(bool mailNotification) {
	_mailNotified = mailNotification;
}

bool MSNAccount::isMailNotified() const {
	return _mailNotified;
}

void MSNAccount::setServer(const std::string & msnServer) {
	_msnServer = msnServer;
}

std::string MSNAccount::getServer() const {
	return _msnServer;
}

void MSNAccount::setServerPort(int msnServerPort) {
	_msnServerPort = msnServerPort;
}

int MSNAccount::getServerPort() const {
	return _msnServerPort;
}

void MSNAccount::setHttpConnection(bool useHttp) {
	_useHttp = useHttp;
}

bool MSNAccount::isHttpUsed() const {
	return _useHttp;
}
