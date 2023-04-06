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

#include <coipmanager_base/account/IAccount.h>

IAccount::IAccount()
	: Peer() {

	_connectionConfigurationSet = false;
	_savePassword = false;
}

IAccount::IAccount(const std::string & login, const std::string & password,
	EnumAccountType::AccountType accountType)
	: Peer(accountType, login) {

	_connectionConfigurationSet = false;
	_savePassword = false;
	_password = password;
}

IAccount::IAccount(const IAccount & iAccount)
	: Peer(iAccount) {

	copy(iAccount);
}

IAccount * IAccount::clone() const {
	return new IAccount(*this);
}

IAccount::~IAccount() {
}

void IAccount::copy(const IAccount & iAccount) {
	Peer::copy(iAccount);

	_connectionConfigurationSet = iAccount._connectionConfigurationSet;
	_savePassword = iAccount._savePassword;
	_password = iAccount._password;
}

std::string IAccount::getLogin() const {
	return Peer::getPeerId();
}

void IAccount::setLogin(const std::string & login) {
	Peer::setPeerId(login);
}

std::string IAccount::getPassword() const {
	return _password;
}

void IAccount::setPassword(const std::string & password) {
	_password = password;
}

void IAccount::setConnectionConfigurationSet(bool set) {
	_connectionConfigurationSet = set;
}

bool IAccount::isConnectionConfigurationSet() const {
	return _connectionConfigurationSet;
}

void IAccount::setSavePassword(bool savePassword) {
	_savePassword = savePassword;
}

bool IAccount::isPasswordSaved() const {
	return _savePassword;
}

void IAccount::setPeerId(const std::string & peerId) {
	Peer::setPeerId(peerId);
}

std::string IAccount::getPeerId() const {
	return Peer::getPeerId();
}

void IAccount::setAccountType(EnumAccountType::AccountType accountType) {
	Peer::setAccountType(accountType);
}
