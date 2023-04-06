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

#include <coipmanager_base/EnumAccountType.h>

#include <util/Logger.h>

#include <map>

typedef std::map<EnumAccountType::AccountType, std::string> AccountTypeMap;
static AccountTypeMap _accountTypeMap;

static void init() {
	_accountTypeMap[EnumAccountType::AccountTypeUnknown] = "Unknown";
	_accountTypeMap[EnumAccountType::AccountTypeAIM] = "AIM";
	_accountTypeMap[EnumAccountType::AccountTypeGTalk] = "GTalk";
	_accountTypeMap[EnumAccountType::AccountTypeIAX] = "IAX";
	_accountTypeMap[EnumAccountType::AccountTypeICQ] = "ICQ";
	_accountTypeMap[EnumAccountType::AccountTypeJabber] = "Jabber";
	_accountTypeMap[EnumAccountType::AccountTypeMSN] = "MSN";
	_accountTypeMap[EnumAccountType::AccountTypeSIP] = "SIP";
	_accountTypeMap[EnumAccountType::AccountTypeWengo] = "Wengo";
	_accountTypeMap[EnumAccountType::AccountTypeYahoo] = "Yahoo";
}

std::string EnumAccountType::toString(AccountType accountType) {
	init();
	std::string tmp = _accountTypeMap[accountType];
	if (tmp.empty()) {
		LOG_FATAL("unknown AccountType=" + QString::number(accountType).toStdString());
	}
	return tmp;
}

EnumAccountType::AccountType EnumAccountType::toAccountType(const std::string & accountType) {
	init();
	for (AccountTypeMap::const_iterator it = _accountTypeMap.begin();
		it != _accountTypeMap.end();
		++it) {

		if ((*it).second == accountType) {
			return (*it).first;
		}
	}

	LOG_FATAL("unknown AccountType=" + accountType);
	return AccountTypeUnknown;
}
