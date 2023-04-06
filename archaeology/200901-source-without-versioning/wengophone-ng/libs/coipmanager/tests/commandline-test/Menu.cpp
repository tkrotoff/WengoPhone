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

#include "Menu.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AccountList.h>
#include <coipmanager/callsessionmanager/CallSessionManager.h>
#include <coipmanager/callsessionmanager/CallSession.h>
#include <coipmanager_base/contact/Contact.h>
#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/Logger.h>
#include <util/String.h>

#include <list>
#include <iostream>

Menu::Menu(const std::string & name) {
	_menuName = name;
}

void Menu::executeMenu() {
	unsigned index = 0;

	if (_menuContent.size() > 0) {
		displayMenu();
		index = waitForUserEntry(_menuContent.size() - 1);

		_menuContent[index].second();
	}
}

void Menu::displayMenu() {
	unsigned i = 0;

	std::cout << std::endl;
	std::cout << _menuName << " menu" << std::endl;

	for (MenuContentType::const_iterator it = _menuContent.begin();
		it != _menuContent.end();
		++it, ++i) {
		std::cout << i << " - " << (*it).first << std::endl;
	}
}

unsigned Menu::waitForUserEntry(unsigned max) {
	unsigned result = 0;

	std::cout << "? ";
	std::cin >> result;

	while (result > max) {
		std::cout << "? ";
		std::cin >> result;
	}

	return result;
}

std::string Menu::waitForUserEntry() {
	std::string result;
	std::cin >> result;
	return result;
}

EnumAccountType::AccountType Menu::chooseAProtocol() {
	unsigned protocolIndex = 0;
	EnumAccountType::AccountType result = EnumAccountType::AccountTypeUnknown;

	// Protocol
	std::cout << "Choose accountType: " << std::endl;
	std::cout << "0 - MSN" << std::endl;
	std::cout << "1 - Yahoo" << std::endl;
	std::cout << "2 - AIM" << std::endl;
	std::cout << "3 - ICQ" << std::endl;
	std::cout << "4 - GTalk" << std::endl;
	std::cout << "5 - Jabber" << std::endl;
	std::cout << "6 - IAX" << std::endl;
	std::cout << "7 - Wengo" << std::endl;
//	std::cout << "8 - SIP" << std::endl;

	protocolIndex = waitForUserEntry(7);

	switch (protocolIndex) {
	case 0:
		result = EnumAccountType::AccountTypeMSN;
		break;
	case 1:
		result = EnumAccountType::AccountTypeYahoo;
		break;
	case 2:
		result = EnumAccountType::AccountTypeAIM;
		break;
	case 3:
		result = EnumAccountType::AccountTypeICQ;
		break;
	case 4:
		result = EnumAccountType::AccountTypeGTalk;
		break;
	case 5:
		result = EnumAccountType::AccountTypeJabber;
		break;
	case 6:
		result = EnumAccountType::AccountTypeIAX;
		break;
	case 7:
		result = EnumAccountType::AccountTypeWengo;
		break;
/*	case 8:
		result = EnumAccountType::AccountTypeSIP;
		break;
*/	default:
		LOG_ERROR(String::fromNumber(protocolIndex) + " is not a valid index");
	}
	////

	return result;
}


std::string Menu::accountToString(const Account & account) {
	return account.getLogin()
		+ " - "
		+ EnumAccountType::toString(account.getAccountType());
}

void Menu::listAccount(CoIpManager & coIpManager) {
	unsigned i = 0;

	std::cout << std::endl;
	AccountList accountList = coIpManager.getUserProfileManager().getUserProfile().getAccountList();
	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		++it, ++i) {
		std::cout << i << " - " << accountToString(*it) << std::endl;
	}
}

Account * Menu::listAccountMenu(CoIpManager & coIpManager) {
	listAccount(coIpManager);

	AccountList accountList = coIpManager.getUserProfileManager().getUserProfile().getAccountList();
	unsigned index = waitForUserEntry(accountList.size() - 1);

	unsigned j = 0;
	AccountList::const_iterator it;
	for (it = accountList.begin();
		j < index;
		++it, ++j) {
	}

	return (*it).clone();
}

std::string Menu::callSessionToString(const CallSession * callSession) {
	return EnumPhoneCallState::toString(callSession->getState());
}

void Menu::listCallSession(CoIpManager & coIpManager) {
	/*unsigned i = 0;

	std::cout << std::endl;
	std::list<CallSession *> callSessionList = coIpManager.getCallSessionManager().getCallSessionList();
	for (std::list<CallSession *>::const_iterator it = callSessionList.begin();
		it != callSessionList.end();
		++it, ++i) {
		std::cout << i << " - " << callSessionToString(*it) << std::endl;
	}*/
}

CallSession * Menu::listCallSessionMenu(CoIpManager & coIpManager) {
	/*listCallSession(coIpManager);

	std::list<CallSession *> callSessionList = coIpManager.getCallSessionManager().getCallSessionList();
	unsigned index = waitForUserEntry(callSessionList.size() - 1);

	unsigned j = 0;
	std::list<CallSession *>::const_iterator it;
	for (it = callSessionList.begin();
		j < index;
		++it, ++j) {
	}

	return *it;*/
	return NULL;
}

std::string Menu::contactToString(const Contact & contact) {
	return contact.getDisplayName();
}

void Menu::listContact(CoIpManager & coIpManager) {
	unsigned i = 0;

	std::cout << std::endl;
	ContactList contacts = coIpManager.getUserProfileManager().getUserProfile().getContactList();
	for (ContactList::const_iterator it = contacts.begin();
		it != contacts.end();
		++it, ++i) {
		std::cout << i << " - " << contactToString(*it) << std::endl;
	}
}

Contact * Menu::listContactMenu(CoIpManager & coIpManager) {
	listContact(coIpManager);

	ContactList contacts = coIpManager.getUserProfileManager().getUserProfile().getContactList();
	unsigned index = waitForUserEntry(contacts.size() - 1);

	unsigned j = 0;
	ContactList::const_iterator it;
	for (it = contacts.begin();
		j < index;
		++it, ++j) {
	}

	return contacts.getContact((*it).getUUID());
}

std::string Menu::imContactToString(const IMContact & imContact) {
	return imContact.getContactId()
		+ " - "
		+ EnumAccountType::toString(imContact.getAccountType());
}

void Menu::listIMContact(Contact & contact) {
	unsigned i = 0;

	std::cout << std::endl;
	IMContactList imContactList = contact.getIMContactList();
	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it, ++i) {
		std::cout << i << " - " << imContactToString(*it) << std::endl;
	}
}

IMContact Menu::listIMContactMenu(Contact & contact) {
	listIMContact(contact);

	IMContactList imContactList = contact.getIMContactList();
	unsigned index = waitForUserEntry(imContactList.size() - 1);

	unsigned j = 0;
	IMContactList::const_iterator it;
	for (it = imContactList.begin();
		j < index;
		++it, ++j) {
	}

	return *it;
}
