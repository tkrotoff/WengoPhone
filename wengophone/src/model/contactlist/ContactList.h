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

#include <imwrapper/EnumPresenceState.h>

#include <Event.h>
#include <Serializable.h>
#include <List.h>

class Contact;
class ContactGroup;
class ContactListDataLayer;
class IMContactListHandler;
class IMContactList;
class IMContact;
class IMPresence;
class PresenceHandler;
class StringList;
class WengoPhone;

/**
 * Contact list.
 *
 * List of ContactGroup.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ContactList : public Serializable {
public:

	ContactList(WengoPhone & wengoPhone);

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
	 * Loads the ContactList from the data layer.
	 */
	void load();

	/**
	 * @see List::add()
	 */
	void addContactGroup(ContactGroup * contactGroup);

	/**
	 * @see List::remove()
	 */
	bool removeContactGroup(ContactGroup * contactGroup);

	/**
	 * Permits to use ContactList as an array.
	 *
	 * @see List::operator[]
	 * @param i index inside the array
	 * @return the ContactGroup that corresponds to the index i inside the ContactList or NULL
	 */
	ContactGroup * operator[](unsigned i) const;

	/**
	 * Retrieves a ContactGroup given its name.
	 *
	 * Comparison is case sensitive.
	 *
	 * @param contactGroupName name of the ContactGroup to retrieve
	 * @return the ContactGroup or NULL if failed to find it
	 */
	ContactGroup * operator[](const std::string & contactGroupName) const;

	/**
	 * Get a Contact from a contact id.
	 *
	 * @param contact the contact id
	 * @return the contact or NULL if not found
	 */
	Contact * getContact(const std::string & contactId) const;

	/**
	 * @return amount of ContactGroup
	 */
	unsigned size() const {
		return _contactGroupList.size();
	}

	/**
	 * Gets the StringList representation of the ContactGroup list.
	 *
	 * @return ContactGroup StringList
	 */
	StringList toStringList() const;

	std::string serialize();

	bool unserialize(const std::string & data);

private:

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
	void presenceStateChangedEventHandler(PresenceHandler & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const IMContact & imContact);

	/** Data layer for the ContactList. */
	ContactListDataLayer * _dataLayer;

	/** Defines the vector of ContactGroup. */
	typedef List<ContactGroup *> ContactGroups;

	/** List of ContactGroup. */
	ContactGroups _contactGroupList;

	WengoPhone & _wengoPhone;

};

#endif	//CONTACTLIST_H
