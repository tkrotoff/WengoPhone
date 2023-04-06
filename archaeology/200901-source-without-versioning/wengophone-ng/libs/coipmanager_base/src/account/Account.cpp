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

#include <coipmanager_base/account/Account.h>

#include <coipmanager_base/account/AIMAccount.h>
#include <coipmanager_base/account/GTalkAccount.h>
#include <coipmanager_base/account/IAXAccount.h>
#include <coipmanager_base/account/ICQAccount.h>
#include <coipmanager_base/account/JabberAccount.h>
#include <coipmanager_base/account/MSNAccount.h>
#include <coipmanager_base/account/SipAccount.h>
#include <coipmanager_base/account/WengoAccount.h>
#include <coipmanager_base/account/YahooAccount.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

Account::Account()
	: IAccount() {

	_privateAccount = NULL;
}

Account::Account(const std::string & login, const std::string & password,
	EnumAccountType::AccountType accountType)
	: IAccount(login, password, accountType) {

	switch (accountType) {
	case EnumAccountType::AccountTypeAIM:
		_privateAccount = new AIMAccount(login, password);
		break;
	case EnumAccountType::AccountTypeGTalk:
		_privateAccount = new GTalkAccount(login, password);
		break;
	case EnumAccountType::AccountTypeIAX:
		_privateAccount = new IAXAccount(login, password);
		break;
	case EnumAccountType::AccountTypeICQ:
		_privateAccount = new ICQAccount(login, password);
		break;
	case EnumAccountType::AccountTypeJabber:
		_privateAccount = new JabberAccount(login, password);
		break;
	case EnumAccountType::AccountTypeMSN:
		_privateAccount = new MSNAccount(login, password);
		break;
	case EnumAccountType::AccountTypeSIP:
		_privateAccount = new SipAccount(login, password);
		break;
	case EnumAccountType::AccountTypeWengo:
		_privateAccount = new WengoAccount(login, password);
		break;
	case EnumAccountType::AccountTypeYahoo:
		_privateAccount = new YahooAccount(login, password);
		break;
	default:
		LOG_FATAL("accountType not supported: " + accountType);
	}
}

Account::Account(const IAccount * iAccount)
	: IAccount() {
	_privateAccount = iAccount->clone();
}

Account::Account(const Account & account)
	: IAccount(account) {
	copy(account);
}

Account & Account::operator = (const Account & account) {
	copy(account);
	return *this;
}

Account * Account::clone() const {
	return new Account(*this);
}

Account::~Account() {
	OWSAFE_DELETE(_privateAccount);
}

void Account::copy(const Account & account) {
	IAccount::copy(account);

	if (account._privateAccount) {
		_privateAccount = account._privateAccount->clone();
	} else {
		_privateAccount = NULL;
	}
}

std::string Account::getLogin() const {
	std::string result;

	if (_privateAccount) {
		result = _privateAccount->getLogin();
	}

	return result;
}

void Account::setLogin(const std::string & login) {
	if (_privateAccount) {
		_privateAccount->setLogin(login);
	}
}

std::string Account::getPassword() const {
	std::string result;

	if (_privateAccount) {
		result = _privateAccount->getPassword();
	}

	return result;
}

void Account::setPassword(const std::string & password) {
	if (_privateAccount) {
		_privateAccount->setPassword(password);
	}
}

void Account::setAccountType(EnumAccountType::AccountType accountType) {
	if (_privateAccount) {
		_privateAccount->setAccountType(accountType);
	}
}

EnumAccountType::AccountType Account::getAccountType() const {
	EnumAccountType::AccountType result = EnumAccountType::AccountTypeUnknown;

	if (_privateAccount) {
		result = _privateAccount->getAccountType();
	}

	return result;
}

std::string Account::getAlias() const {
	std::string result;

	if (_privateAccount) {
		result = _privateAccount->getAlias();
	}

	return result;
}

void Account::setAlias(const std::string & alias) {
	if (_privateAccount) {
		_privateAccount->setAlias(alias);
	}
}

void Account::setPresenceState(EnumPresenceState::PresenceState presenceState) {
	if (_privateAccount) {
		_privateAccount->setPresenceState(presenceState);
	}
}

EnumPresenceState::PresenceState Account::getPresenceState() const {
	EnumPresenceState::PresenceState result = EnumPresenceState::PresenceStateUnknown;

	if (_privateAccount) {
		result = _privateAccount->getPresenceState();
	}

	return result;
}

void Account::setConnectionState(EnumConnectionState::ConnectionState state) {
	if (_privateAccount) {
		_privateAccount->setConnectionState(state);
	}
}

EnumConnectionState::ConnectionState Account::getConnectionState() const {
	EnumConnectionState::ConnectionState result = EnumConnectionState::ConnectionStateDisconnected;

	if (_privateAccount) {
		result = _privateAccount->getConnectionState();
	}

	return result;
}

void Account::setConnectionConfigurationSet(bool set) {
	if (_privateAccount) {
		_privateAccount->setConnectionConfigurationSet(set);
	}
}

bool Account::isConnectionConfigurationSet() const {
	bool result = false;

	if (_privateAccount) {
		result = _privateAccount->isConnectionConfigurationSet();
	}

	return result;
}

void Account::setSavePassword(bool savePassword) {
	if (_privateAccount) {
		_privateAccount->setSavePassword(savePassword);
	}
}

bool Account::isPasswordSaved() const {
	bool result = false;

	if (_privateAccount) {
		result = _privateAccount->isPasswordSaved();
	}

	return result;
}

IAccount * Account::getPrivateAccount() const {
	return _privateAccount;
}
