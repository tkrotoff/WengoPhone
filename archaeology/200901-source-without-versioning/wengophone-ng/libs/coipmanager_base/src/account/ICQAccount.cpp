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

#include <coipmanager_base/account/ICQAccount.h>

#include <util/Logger.h>

ICQAccount::ICQAccount()
	: OscarAccount() {
	_accountType = EnumAccountType::AccountTypeICQ;
}

ICQAccount::ICQAccount(const std::string & login, const std::string & password) 
	: OscarAccount(login, password) {
	_accountType = EnumAccountType::AccountTypeICQ;
}

ICQAccount::ICQAccount(const ICQAccount & account)
	: OscarAccount(account) {
}

ICQAccount::ICQAccount(const IAccount * iAccount) {
	const ICQAccount * account = dynamic_cast<const ICQAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an ICQAccount");
	}
}

ICQAccount::~ICQAccount() {
}

ICQAccount * ICQAccount::clone() const {
	return new ICQAccount(*this);
}

void ICQAccount::copy(const ICQAccount & account) {
	OscarAccount::copy(account);
}
