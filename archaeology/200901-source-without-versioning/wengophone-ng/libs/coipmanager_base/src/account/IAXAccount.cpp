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

#include <coipmanager_base/account/IAXAccount.h>

#include <util/Logger.h>
#include <util/String.h>

static const int DEFAULT_IAX_PORT = 4569;

IAXAccount::IAXAccount()
	: SipAccount() {
	_accountType = EnumAccountType::AccountTypeIAX;
	_sipProxyServerPort = DEFAULT_IAX_PORT;
}

IAXAccount::IAXAccount(const std::string & login, const std::string & password)
	: SipAccount() {
	setLogin(login);
	_password = password;
	_accountType = EnumAccountType::AccountTypeIAX;
	_sipProxyServerPort = DEFAULT_IAX_PORT;
}

IAXAccount::IAXAccount(const IAXAccount & account)
	: SipAccount(account) {
	copy(account);
}

IAXAccount::IAXAccount(const IAccount * iAccount) {
	const IAXAccount * account = dynamic_cast<const IAXAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an IAXAccount");
	}
}

IAXAccount::~IAXAccount() {
}

IAXAccount * IAXAccount::clone() const {
	return new IAXAccount(*this);
}

void IAXAccount::copy(const IAXAccount & account) {
	IAccount::copy(account);
}

std::string IAXAccount::getIdentity() const {
	return getLogin();
}

std::string IAXAccount::getDisplayName() const {
	return getLogin();
}
