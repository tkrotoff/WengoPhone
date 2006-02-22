/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef IMCONTACTLIST_H
#define IMCONTACTLIST_H

#include <string>

class IMAccount;

class IMContactList {
public:

	/**
	* Emitted when an IMContact has been added remotely.
	 *
	 * @param sender this class
	 * @param contactId the new contact
	 */
	Event< void (IMContactList & sender, const std::string & contactId) > newContactAddedEvent;
	
	/**
	* Emitted when an IMContact has been removed remotely.
	 *
	 * @param sender this class
	 * @param contactId the removed contact
	 */
	Event< void (IMContactList & sender, const std::string & contactId) > contactRemovedEvent;

	IMContactList(const IMAccount & imAccount) 
		: _imAccount(imAccount) {}

	virtual ~IMContactList();

	/**
	 * Remotely adds a contact.
	 *
	 * @param contactId the contact to add
	 */
	virtual void addContact(const std::string & contactId) = 0;
	
	/**
	 * Remotely deletes a contact.
	 *
	 * @param contactId the contact to remove
	 */
	virtual void removeContact(const std::string & contactId) = 0;

	/**
	 * @return the associated IMAccount.
	 */
	const IMAccount & getIMAccount() const {
		return _imAccount;
	}

private:

	const IMAccount & _imAccount;

};

#endif //IMCONTACTLIST_H