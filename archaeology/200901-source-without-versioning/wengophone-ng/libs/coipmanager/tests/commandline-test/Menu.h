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

#ifndef OWMENU_H
#define OWMENU_H

#include <coipmanager_base/EnumAccountType.h>

#include <util/Interface.h>

#include <boost/function.hpp>

#include <string>
#include <vector>

class CallSession;
class Account;
class CoIpManager;
class Contact;
class IMContact;

/**
 * Menu interface.
 *
 * @author Philippe Bernery
 */
class Menu : Interface {
public:

	Menu(const std::string & name);

protected:

	/**
	 * Displays the menu and waits for user entry.
	 */
	void executeMenu();

	/**
	 * Displays the menu of this command.
	 */
	void displayMenu();

	/**
	 * Waits for user entry and return the index entered.
	 *
	 * @param max the maximun authorized index. If the index entered
	 * is greater than 'max', user is asked again for index.
	 */
	static unsigned waitForUserEntry(unsigned max);

	/**
	 * Waits for user entry and return string entered.
	 */
	static std::string waitForUserEntry();

	/**
	 * @name Useful methods
	 * @{
	 */

	/**
	 * Displays a menu to select a Protocol.
	 *
	 * @return the choosed accountType
	 */
	static EnumAccountType::AccountType chooseAProtocol();

	/** Produces a human readable string representing an account. */
	static std::string accountToString(const Account & account);

	/**
	 * Displays the list of Account.
	 */
	static void listAccount(CoIpManager & coIpManager);

	/**
	 * Displays the list of Account and asked the user to choose one.
	 * Return the one selected.
	 */
	static Account * listAccountMenu(CoIpManager & coIpManager);

	/** Produces a human readable string representing a callSession. */
	static std::string callSessionToString(const CallSession * callSession);

	/**
	 * Displays the list of Call Sessions
	 */
	static void listCallSession(CoIpManager & coIpManager);

	/**
	 * Displays the list of  Call Sessions and asked the user to choose one.
	 * Return the one selected.
	 */
	static CallSession * listCallSessionMenu(CoIpManager & coIpManager);

	/** Produces a human readable string representing a contact. */
	static std::string contactToString(const Contact & contact);

	/**
	 * Displays the list of Contact.
	 */
	static void listContact(CoIpManager & coIpManager);

	/**
	 * Displays the list of Contact and asked the user to choose one.
	 * Return the one selected.
	 */
	static Contact * listContactMenu(CoIpManager & coIpManager);

	/**
	 * Creates a human readable string from an IMContact
	 */
	static std::string imContactToString(const IMContact & imContact);

	/**
	 * Displays the list of IMContact.
	 */
	static void listIMContact(Contact & contact);

	/**
	 * Displays the list of IMContact and asked the user to choose one.
	 * Return the one selected.
	 */
	static IMContact listIMContactMenu(Contact & contact);

	/**
	 * @}
	 */

	/** Menu content. */
	typedef std::vector< std::pair< std::string, boost::function< void () > > > MenuContentType;
	MenuContentType _menuContent;

	/** The Menu name. */
	std::string _menuName;

};

#endif //OWMENU_H
