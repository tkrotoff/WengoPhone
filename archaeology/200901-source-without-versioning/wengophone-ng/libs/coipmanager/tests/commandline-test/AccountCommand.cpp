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

#include "AccountCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager_base/account/AIMAccount.h>
#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/account/GTalkAccount.h>
#include <coipmanager_base/account/IAXAccount.h>
#include <coipmanager_base/account/ICQAccount.h>
#include <coipmanager_base/account/JabberAccount.h>
#include <coipmanager_base/account/MSNAccount.h>
#include <coipmanager_base/account/YahooAccount.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <event/Event2.h>

#include <iostream>

AccountCommand::AccountCommand(CoIpManager & coIpManager)
	: Command("Account management", "Add, Remove and list accounts", coIpManager) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Add an account", boost::bind(&AccountCommand::addAccount, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Remove an account", boost::bind(&AccountCommand::removeAccount, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("List account", boost::bind(&AccountCommand::listAccount, this)));
}

AccountCommand::~AccountCommand() {
}

void AccountCommand::addAccount() {
	std::string login;
	std::string password;
	std::string server;

	EnumAccountType::AccountType accountType = EnumAccountType::AccountTypeUnknown;

	accountType = chooseAProtocol();

	std::cout << "login: ";
	std::cin >> login;

	std::cout << "password: ";
	std::cin >> password;

	Account * account = new Account(login, password, accountType);
	account->setSavePassword(true);
	if (accountType == EnumAccountType::AccountTypeIAX) {
		std::cout << "server: ";
		std::cin >> server;
		((IAXAccount *) account->getPrivateAccount())->setSIPProxyServerHostname(server);
	}

	_coIpManager.getUserProfileManager().getAccountManager().add(*account);
	OWSAFE_DELETE(account);
}

void AccountCommand::removeAccount() {
	Account * account = Menu::listAccountMenu(_coIpManager);
	_coIpManager.getUserProfileManager().getAccountManager().remove(account->getUUID());
	OWSAFE_DELETE(account);
}

void AccountCommand::listAccount() {
	Menu::listAccount(_coIpManager);
}
