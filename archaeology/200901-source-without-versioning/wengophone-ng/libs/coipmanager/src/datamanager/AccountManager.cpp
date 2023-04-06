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

#include <coipmanager/datamanager/AccountManager.h>

#include <coipmanager/AccountMutator.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

#include <algorithm>

class AccountEqualsPredicate : public std::binary_function<Account, Account, bool> {
public:
	bool operator()(Account c1, Account c2) const {
		return ((c1.getLogin() == c2.getLogin())
			&& (c1.getAccountType() == c2.getAccountType()));
	}
};

AccountManager::AccountManager(AccountList & accountList)
	: _accountList(accountList) {

	_mutex = new QMutex(QMutex::Recursive);

	SAFE_CONNECT(&AccountMutator::getInstance(),
		SIGNAL(accountUpdatedSignal(Account, EnumUpdateSection::UpdateSection)),
		SLOT(accountUpdatedSlot(Account, EnumUpdateSection::UpdateSection)));
}

AccountManager::~AccountManager() {
	OWSAFE_DELETE(_mutex);
}

bool AccountManager::add(const Account & account) {
	QMutexLocker lock(_mutex);

	bool result = false;

	// Checks if an Account with same login/accountType already exists
	AccountList::const_iterator it =
		std::find_if(_accountList.begin(), _accountList.end(),
			std::bind2nd(AccountEqualsPredicate(), account));

	if (it == _accountList.end()) {
		_accountList.push_back(account);
		accountAddedSignal(account.getUUID());
		result = true;
	}

	return result;
}

bool AccountManager::remove(const std::string & accountId) {
	QMutexLocker lock(_mutex);

	bool result = false;

	for (AccountList::iterator it = _accountList.begin();
		it != _accountList.end();
		++it) {
		if ((*it).getUUID() == accountId) {
			_accountList.erase(it);
			accountRemovedSignal(accountId);
			result = true;
			break;
		}
	}

	return result;
}

bool AccountManager::update(const Account & account, EnumUpdateSection::UpdateSection section) {
	QMutexLocker lock(_mutex);

	bool result = false;

	AccountList::iterator it = findAccount(_accountList, account);
	if (it != _accountList.end()) {
		switch(section) {
		case EnumUpdateSection::UpdateSectionPresenceState:
			(*it).setPresenceState(account.getPresenceState());
			(*it).setConnectionState(account.getConnectionState());
			break;
		default:
			(*it) = account;
			break;	
		}
		accountUpdatedSignal(account.getUUID(), section);
		result = true;
	}

	return result;
}

AccountList::iterator AccountManager::findAccount(AccountList & accountList, const Account & account) {
	AccountList::iterator result;

	for (result = accountList.begin(); result != accountList.end(); ++result) {
		if ((*result) == account) {
			break;
		}
	}

	return result;
}

void AccountManager::accountUpdatedSlot(Account account, EnumUpdateSection::UpdateSection section) {
	update(account, section);
}
