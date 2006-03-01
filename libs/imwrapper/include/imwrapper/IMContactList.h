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

#ifndef IMCONTACTLIST_H
#define IMCONTACTLIST_H

#include <Interface.h>
#include <Event.h>

#include <string>

class IMAccount;
class IMContact;

class IMContactList : Interface {
public:

	/**
	 * Emitted when an IMContact has been added remotely.
	 *
	 * @param sender this class
	 * @param contactId the new contact
	 */
	Event< void (IMContactList & sender, const std::string & groupName, const std::string & contactId) > newContactAddedEvent;

	/**
	 * Emitted when an IMContact has been removed remotely.
	 *
	 * @param sender this class
	 * @param contactId the removed contact
	 */
	Event< void (IMContactList & sender, const std::string & groupName, const std::string & contactId) > contactRemovedEvent;

	/**
	 * Emitted when an IMContact has been moved remotely.
	 *
	 * @param sender this class
	 * @param contactId the moved contact
	 */
	Event< void (IMContactList & sender, const std::string & groupName, const std::string & contactId) > contactMovedEvent;

	/**
	 * Emitted when a contact state has been changed.
	 *
	 * @param sender this class
	 * @param groupName group name of the contact
	 * @param imContact the changed IMContact
	 */
	Event< void (IMContactList & sender, const std::string & groupName, const IMContact & imContact) > imContactChangedEvent;

	/**
	 * Emitted when an IMContact has been added remotely.
	 *
	 * @param sender this class
	 * @param groupName the added group
	 */
	Event< void (IMContactList & sender, const std::string & groupName) > newContactGroupAddedEvent;

	/**
	 * Emitted when a group has been removed.
	 *
	 * @param sender this class
	 * @param groupName the removed group
	 */
	Event< void (IMContactList & sender, const std::string & groupName) > contactGroupRemovedEvent;

	virtual ~IMContactList() {};

	/**
	 * Remotely adds a contact.
	 *
	 * If the group does not exist, it is automatically created.
	 * @param groupName the group name to add the contact in
	 * @param contactId the contact to add
	 */
	virtual void addContact(const std::string & groupName, const std::string & contactId) = 0;

	/**
	 * Remotely deletes a contact.
	 *
	 * @param groupName the group name of the contact
	 * @param contactId the contact to remove
	 */
	virtual void removeContact(const std::string & groupName, const std::string & contactId) = 0;

	/**
	 * Remotely adds a group.
	 *
	 * @param groupName the group name to add the contact in
	 */
	virtual void addGroup(const std::string & groupName) = 0;

	/**
	 * Remotely deletes a group.
	 *
	 * @param groupName the group name of the contact
	 */
	virtual void removeGroup(const std::string & groupName) = 0;

	/**
	 * Move a contact to another group.
	 *
	 * @param newGroupName the new group name
	 * @param oldGroupName the old group name
	 * @param contactId the contact to move
	 */
	virtual void moveContactToGroup(const std::string & newGroupName,
		const std::string & oldGroupName, const std::string & contactId) = 0;

	/**
	 * Change group name.
	 *
	 * @param oldGroupName the old group name
	 * @param newGroupName the new group name
	 */
	virtual void changeGroupName(const std::string & oldGroupName, const std::string & newGroupName) = 0;

	/**
	 * @return the associated IMAccount.
	 */
	const IMAccount & getIMAccount() const {
		return _imAccount;
	}

protected:

	IMContactList(const IMAccount & imAccount) 
		: _imAccount(imAccount) {}

	const IMAccount & _imAccount;

};

#endif //IMCONTACTLIST_H
