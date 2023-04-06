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

#include <coipmanager_base/account/GTalkAccount.h>

#include <util/Logger.h>

static const std::string DEFAULT_GTALK_SERVER = "talk.google.com";
static const bool DEFAULT_GTALK_USE_TLS = true;
static const bool DEFAULT_GTALK_REQUIRE_TLS = true;

GTalkAccount::GTalkAccount() 
	: JabberAccount() {

	_jabberServer = DEFAULT_GTALK_SERVER;
	_tlsUsed = DEFAULT_GTALK_USE_TLS;
	_tlsRequired = DEFAULT_GTALK_REQUIRE_TLS;
	_connectionServer = DEFAULT_GTALK_SERVER;
	_accountType = EnumAccountType::AccountTypeGTalk;
}

GTalkAccount::GTalkAccount(const std::string & login, const std::string & password) 
	: JabberAccount(login, password) {

	_jabberServer = DEFAULT_GTALK_SERVER;
	_tlsUsed = DEFAULT_GTALK_USE_TLS;
	_tlsRequired = DEFAULT_GTALK_REQUIRE_TLS;
	_connectionServer = DEFAULT_GTALK_SERVER;
	_accountType = EnumAccountType::AccountTypeGTalk;
}

GTalkAccount::GTalkAccount(const GTalkAccount & account) 
	: JabberAccount(account) {
}

GTalkAccount::GTalkAccount(const IAccount * iAccount) {
	const GTalkAccount * account = dynamic_cast<const GTalkAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an GTalkAccount");
	}
}

GTalkAccount::~GTalkAccount() {
}

GTalkAccount * GTalkAccount::clone() const {
	return new GTalkAccount(*this);
}

void GTalkAccount::copy(const GTalkAccount & account) {
	JabberAccount::copy(account);
}
