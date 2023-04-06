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

#include <coipmanager_base/account/JabberAccount.h>

#include <util/Logger.h>
#include <util/String.h>

const std::string JabberAccount::DEFAULT_JABBER_SERVER = String::null;
const int JabberAccount::DEFAULT_JABBER_PORT = 5222;
const std::string JabberAccount::DEFAULT_JABBER_RESOURCE = "WengoPhone";
const bool JabberAccount::DEFAULT_JABBER_USE_TLS = true;
const bool JabberAccount::DEFAULT_JABBER_REQUIRE_TLS = true;
const bool JabberAccount::DEFAULT_JABBER_USE_OLD_SSL = false;
const bool JabberAccount::DEFAULT_JABBER_AUTH_PLAIN_IN_CLEAR = false;
const std::string JabberAccount::DEFAULT_JABBER_CONNECTION_SERVER = String::null;

JabberAccount::JabberAccount()
	: IAccount(String::null, String::null, EnumAccountType::AccountTypeJabber) {

	_jabberServer = DEFAULT_JABBER_SERVER;
	_jabberServerPort = DEFAULT_JABBER_PORT;
	_resource = DEFAULT_JABBER_RESOURCE;
	_tlsUsed = DEFAULT_JABBER_USE_TLS;
	_tlsRequired = DEFAULT_JABBER_REQUIRE_TLS;
	_oldSSLUsed = DEFAULT_JABBER_USE_OLD_SSL;
	_authPlainInClearUsed = DEFAULT_JABBER_AUTH_PLAIN_IN_CLEAR;
	_connectionServer = DEFAULT_JABBER_CONNECTION_SERVER;
}

JabberAccount::JabberAccount(const std::string & login, const std::string & password)
	: IAccount(login, password, EnumAccountType::AccountTypeJabber) {

	_jabberServer = DEFAULT_JABBER_SERVER;
	_jabberServerPort = DEFAULT_JABBER_PORT;
	_resource = DEFAULT_JABBER_RESOURCE;
	_tlsUsed = DEFAULT_JABBER_USE_TLS;
	_tlsRequired = DEFAULT_JABBER_REQUIRE_TLS;
	_oldSSLUsed = DEFAULT_JABBER_USE_OLD_SSL;
	_authPlainInClearUsed = DEFAULT_JABBER_AUTH_PLAIN_IN_CLEAR;
	_connectionServer = DEFAULT_JABBER_CONNECTION_SERVER;
}

JabberAccount::JabberAccount(const JabberAccount & account)
	: IAccount(account) {
	copy(account);
}

JabberAccount::JabberAccount(const IAccount * iAccount) {
	const JabberAccount * account = dynamic_cast<const JabberAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an JabberAccount");
	}
}

void JabberAccount::copy(const JabberAccount & account) {
	IAccount::copy(account);
	_jabberServer = account._jabberServer;
	_jabberServerPort = account._jabberServerPort;
	_resource = account._resource;
	_tlsUsed = account._tlsUsed;
	_tlsRequired = account._tlsRequired;
	_oldSSLUsed = account._oldSSLUsed;
	_authPlainInClearUsed = account._authPlainInClearUsed;
	_connectionServer = account._connectionServer;
}

JabberAccount::~JabberAccount() {
}

JabberAccount * JabberAccount::clone() const {
	return new JabberAccount(*this);
}

void JabberAccount::setServer(const std::string & jabberServer) {
	_jabberServer = jabberServer;
}

std::string JabberAccount::getServer() const {
	return _jabberServer;
}

void JabberAccount::setServerPort(int jabberServerPort) {
	_jabberServerPort = jabberServerPort;
}

int JabberAccount::getServerPort() const {
	return _jabberServerPort;
}

void JabberAccount::setResource(const std::string & resource) {
	_resource = resource;
}

std::string JabberAccount::getResource() const {
	return _resource;
}

void JabberAccount::setTLS(bool useTLS) {
	_tlsUsed = useTLS;
}

bool JabberAccount::isTLSUsed() const {
	return _tlsUsed;
}

void JabberAccount::setTLSRequired(bool requireTLS) {
	_tlsRequired = requireTLS;
}

bool JabberAccount::isTLSRequired() const {
	return _tlsRequired;
}

void JabberAccount::setOldSSLUsed(bool oldSSLUsed) {
	_oldSSLUsed = oldSSLUsed;
}

bool JabberAccount::isOldSSLUsed() const {
	return _oldSSLUsed;
}

void JabberAccount::setAuthPlainInClearUsed(bool useAuthPlainInClear) {
	_authPlainInClearUsed = useAuthPlainInClear;
}

bool JabberAccount::isAuthPlainInClearUsed() const {
	return _authPlainInClearUsed;
}

void JabberAccount::setConnectionServer(const std::string & connectionServer) {
	_connectionServer = connectionServer;
}

std::string JabberAccount::getConnectionServer() const {
	return _connectionServer;
}
