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

#include "ContactListCommand.h"

#include "ContactCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <iostream>

ContactListCommand::ContactListCommand(CoIpManager & coIpManager)
	: Command("Contact list management", "Add, remove and list contacts", coIpManager) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Add a contact", boost::bind(&ContactListCommand::addContact, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Remove a contact", boost::bind(&ContactListCommand::removeContact, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Modify a contact", boost::bind(&ContactListCommand::modifyContact, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("List contacts", boost::bind(&ContactListCommand::listContact, this)));
}

ContactListCommand::~ContactListCommand() {
}

void ContactListCommand::addContact() {
	Contact contact;

	ContactCommand contactCommand(contact, _coIpManager);
	contactCommand.exec();

	_coIpManager.getUserProfileManager().getContactManager().add(contact);
}

void ContactListCommand::modifyContact() {
	Contact * contact = Menu::listContactMenu(_coIpManager);

	ContactCommand contactCommand(*contact, _coIpManager);
	contactCommand.exec();
}

void ContactListCommand::removeContact() {
	Contact * contact = Menu::listContactMenu(_coIpManager);
	_coIpManager.getUserProfileManager().getContactManager().remove(contact->getUUID());
}

void ContactListCommand::listContact() {
	Menu::listContact(_coIpManager);
}
