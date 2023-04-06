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

#include <coipmanager_base/account/AIMAccount.h>

#include <util/Logger.h>

AIMAccount::AIMAccount()
	: OscarAccount() {
	_accountType = EnumAccountType::AccountTypeAIM;
}

AIMAccount::AIMAccount(const std::string & login, const std::string & password) 
	: OscarAccount(login, password) {
	_accountType = EnumAccountType::AccountTypeAIM;
}

AIMAccount::AIMAccount(const AIMAccount & account)
	: OscarAccount(account) {
}

AIMAccount::AIMAccount(const IAccount * iAccount) {
	const AIMAccount * account = dynamic_cast<const AIMAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an AIMAccount");
	}
}

AIMAccount::~AIMAccount() {
}

AIMAccount * AIMAccount::clone() const {
	return new AIMAccount(*this);
}

void AIMAccount::copy(const AIMAccount & account) {
	OscarAccount::copy(account);
}
