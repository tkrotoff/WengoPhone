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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include "ContactGroup.h"

#include <imwrapper/EnumPresenceState.h>

#include <util/Event.h>

#include <set>
#include <list>

class Contact;
class IMAccount;
class IMContact;
class IMContactList;
class IMContactListHandler;
class IMPresence;
class PresenceHandler;
class StringList;
class UserProfile;

/**
 * Contact list.
 *
 * List of ContactGroup.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ContactList {
	friend class Contact;
	friend class ContactListXMLSerializer;
public:

	/** Set of ContactGroup. */
	typedef std::set<ContactGroup> ContactGroupSet;

	ContactList(UserProfile & userProfile);

	~ContactList();

	/**
	 * A ContactGroup has been added.
	 *
	 * @param sender this class
	 * @param contactGroup ContactGroup added
	 */
	Event<void (ContactList & sender, ContactGroup & contactGroup)> contactGroupAddedEvent;

	/**
	 * A ContactGroup has been removed.
	 *
	 * @param sender this class
	 * @param contactGroup ContactGroup removed
	 */
	Event<void (ContactList & sender, ContactGroup & contactGroup)> contactGroupRemovedEvent;

	/**
	 * A Contact has been added to a ContactGroup.
	 *
	 * @param sender this class
	 * @param contactGroup the ContactGroup where the Contact has been added to
	 * @param contact the added Contact
	 */
	Event<void (ContactList & sender, Contact & contact)> contactAddedEvent;

	/**
	 * A Contact has been removed from a ContactGroup.
	 *
	 * @param sender this class
	 * @param contactGroup the ContactGroup where the Contact has been removed from
	 * @param contact the removed Contact
	 */
	Event<void (ContactList & sender, Contact & contact)> contactRemovedEvent;

	/**
	 * Add a ContactGroup.
	 *
	 * If a ContactGroup with the given name exists, no ContactGroup is created
	 *
	 * @param name of the ContactGroup to add.
	 */
	void addContactGroup(const std::string & name);

	/**
	 * Remove a ContactGroup.
	 *
	 * If no ContactGroup with the given name exists, nothing happens
	 *
	 * @param name of the ContactGroup to remove.
	 */
	void removeContactGroup(const std::string & name);

	/**
	 * Create and add a Contact to the ContactList.
	 *
	 * @return a reference to the new Contact
	 */
	Contact & createContact();

	/**
	 * Remove a Contact.
	 *
	 * @param the contact to remove
	 */
	void removeContact(const Contact & contact);

	/**
	 * Get a ContactGroup.
	 *
	 * @param groupName the name of the desired ContactGroup
	 * @return a pointer to the ContactGroup, NULL if not found
	 */
	ContactGroup * getContactGroup(const std::string & groupName) const;

	/**
	 * @return a copy of the set of ContactGroups.
	 */
	ContactGroupSet getContactGroupSet() const {
		return ContactGroupSet(_contactGroupSet);
	}

	/**
	 * Find the first Contact that owns an IMContact.
	 *
	 * @param imContact the IMContact to look for
	 * @return the Contact or NULL if not found
	 */
	Contact * findContactThatOwns(const IMContact & imContact) const;

private:

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see IMAccount::imAccountDeadEvent
	 */
	void imAccountDeadEventHandler(IMAccount & sender);

	/**
	 * @see IMContactListHandler::newIMContactAddedEvent
	 */
	void newIMContactAddedEventHandler(IMContactListHandler & sender,
		const std::string & groupName, IMContact & newIMContact);

	/**
	 * @see IMContactListHandler::imContactRemovedEvent
	 */
	void imContactRemovedEventHandler(IMContactListHandler & sender,
		const std::string & groupName, IMContact & imContact);

	/**
	 * @see IMContactListHandler::newContactGroupAddedEvent
	 */
	void newContactGroupAddedEventHandler(IMContactList & sender,
		const std::string & groupName);

	/**
	 * @see IMContactListHandler::contactGroupRemovedEvent
	 */
	void contactGroupRemovedEventHandler(IMContactList & sender,
		const std::string & groupName);

	/**
	 * @see PresenceHandler::presenceStateChangedEventHandler
	 */
	void presenceStateChangedEventHandler(PresenceHandler & sender,
		EnumPresenceState::PresenceState state,
		const std::string & alias, const IMContact & imContact);

	/**
	 * @see IMContactListHandler::imContactMovedEvent
	 */
	void imContactMovedEventHandler(IMContactListHandler & sender,
		const std::string & groupName, IMContact & imContact);

	/**
	 * Add an IMContact to a Contact.
	 *
	 * It sends an asynchronous command to IMContactListHandler.
	 *
	 * Must only be called by Contact
	 *
	 * @param contact the Contact that wants to add an IMContact
	 * @param imContact the IMContact to add
	 */
	void addIMContact(Contact & contact, const IMContact & imContact);

	/**
	 * Remove an IMContact from a Contact.
	 *
	 * It sends an asynchronous command to IMContactListHandler.
	 *
	 * Must only be called by Contact
	 *
	 * @param contact the Contact that wants to remove an IMContact
	 * @param imContact the IMContact to remove
	 */
	void removeIMContact(Contact & contact, const IMContact & imContact);

	/**
	 * Add a Contact to a ContactGroup.
	 *
	 * It sends an asynchronous command to IMContactListHandler.
	 *
	 * This method must be called only by Contact
	 *
	 * @param groupName the group name
	 * @param contact the Contact to add
	 */
	void addToContactGroup(const std::string & groupName, Contact & contact);

	/**
	 * Remove a Contact from a ContactGroup.
	 *
	 * It sends an asynchronous command to IMContactListHandler.
	 *
	 * This method must be called only by Contact
	 *
	 * @param groupName the group name to remove from
	 * @param contact the Contact that wants to be removed from the ContactGroup
	 */
	void removeFromContactGroup(const std::string & groupName, Contact & contact);

	/**
	 * Move a Contact to another ContactGroup.
	 *
	 * @param contact the Contact to move
	 * @param to the group where we want to move in the Contact
	 * @param from the group where we want to move out the Contact
	 */
	void moveContactToGroup(Contact & contact, const std::string & to, const std::string & from);

	/**
	 * Actually add a ContactGroup.
	 *
	 * @param groupName the group name
	 */
	void _addContactGroup(const std::string & groupName);

	/**
	 * Actually remove a ContactGroup.
	 *
	 * @param groupName the group name
	 */
	void _removeContactGroup(const std::string & groupName);

	/**
	 * Actually add the Contact to a ContactGroup.
	 *
	 * @param contact the Contact
	 * @param groupName the ContactGroup
	 */
	void _addToContactGroup(const std::string & groupName, Contact & contact);

	/**
	 * Actually remove the Contact to a ContactGroup.
	 *
	 * @param contact the Contact
	 * @param groupName the ContactGroup
	 */
	void _removeFromContactGroup(const std::string & groupName, Contact & contact);

	/** List of Contact. */
	typedef std::list<Contact> Contacts;

	/** Set of ContactGroup. */
	ContactGroupSet _contactGroupSet;

	/** Vector of Contact. */
	Contacts _contacts;

	UserProfile & _userProfile;

	IMContactListHandler & _imContactListHandler;
};

#endif	//CONTACTLIST_H
