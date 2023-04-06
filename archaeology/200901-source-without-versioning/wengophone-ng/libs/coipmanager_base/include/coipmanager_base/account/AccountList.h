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

#ifndef OWACCOUNTLIST_H
#define OWACCOUNTLIST_H

#include <coipmanager_base/account/Account.h>

#include <list>

/**
 * Account list.
 *
 * Account can be ordered manually, order will be kept.
 *
 * @author Philippe Bernery
 */
typedef std::list<Account> AccountList;

/**
 * AccounList helper methods.
 *
 * Defines some useful method.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API AccountListHelper {
public:

	/**
	 * Gets a list of given accountType.
	 */
	static AccountList getCopyOfAccountsOfProtocol(const AccountList & accountList, 
		EnumAccountType::AccountType accountType);

	/**
	 * Gets a copy of the Account identified by the given UUID.
	 *
	 * @return NULL if not found, a pointer to the Account otherwise.
	 */
	static Account * getCopyOfAccount(const AccountList & accountList, const std::string & uuid);

	/**
	 * Gets the first Account that has given login/accountType.
	 *
	 * @return a clone of the found Account, NULL otherwhise
	 */
	static Account * getCopyOfAccount(const AccountList & accountList, 
		const std::string & login, EnumAccountType::AccountType accountType);
};

#endif //OWACCOUNTLIST_H
