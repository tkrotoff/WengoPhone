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

#include "ContactCommand.h"

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/contact/Contact.h>
#include <coipmanager_base/imcontact/IMContactList.h>

#include <util/SafeDelete.h>
#include <event/Event2.h>

#include <iostream>

ContactCommand::ContactCommand(Contact & contact, CoIpManager & coIpManager)
	: Command("Contact management", "Add, remove and list IMContacts", coIpManager),
	_contact(contact) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Add an IMContact", boost::bind(&ContactCommand::addIMContact, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Remove an IMContact", boost::bind(&ContactCommand::removeIMContact, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("List IMContacts", boost::bind(&ContactCommand::listIMContactCommand, this)));
}

ContactCommand::~ContactCommand() {
}

void ContactCommand::addIMContact() {
	std::cout << "Add an IMContact" << std::endl;

	std::string contactId;
	std::cout << "contact id: ";
	std::cin >> contactId;

	std::cout << "Choose the account to associate: " << std::endl;
	Account * account = Menu::listAccountMenu(_coIpManager);

	IMContact imContact(account->getAccountType(), contactId);
	imContact.setAccountId(account->getUUID());
	_contact.addIMContact(imContact);
	OWSAFE_DELETE(account);
}

void ContactCommand::removeIMContact() {
	IMContact imContact = listIMContactMenu(_contact);
	_contact.removeIMContact(imContact.getUUID());
}

void ContactCommand::listIMContactCommand() {
	listIMContact(_contact);
}
