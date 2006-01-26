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

#ifndef CONTACTGROUP_H
#define CONTACTGROUP_H

#include <Event.h>
#include <Serializable.h>
#include <List.h>

#include <string>

class Contact;

/**
 * A group of contacts (i.e familly, friends ect...).
 *
 * Contains a list of Contact.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ContactGroup : public Serializable {
public:

	/**
	 * A ContactGroup has been modified/updated.
	 *
	 * @param sender ContactGroup modified/updated
	 */
	Event < void(ContactGroup & sender) > contactGroupModifiedEvent;

	/**
	 * A Contact has been added.
	 *
	 * @param sender this class
	 * @param contact Contact added
	 */
	Event < void(ContactGroup & sender, Contact & contact) > contactAddedEvent;

	/**
	 * A Contact has been removed.
	 *
	 * @param sender this class
	 * @param contact Contact removed
	 */
	Event < void(ContactGroup & sender, Contact & contact) > contactRemovedEvent;

	ContactGroup(const std::string & groupName);

	void setName(const std::string & groupName) {
		_groupName = groupName;
	}

	std::string toString() const {
		return _groupName;
	}

	/**
	 * @see List::add()
	 */
	void addContact(Contact * contact);

	/**
	 * @see List::remove()
	 */
	bool removeContact(Contact * contact);

	/**
	 * Permits to use ContactGroup as an array.
	 *
	 * @see List::operator[]
	 * @param i index inside the array
	 * @return the Contact that corresponds to the index i inside the ContactGroup or NULL
	 */
	Contact * operator[](unsigned i) const;

	/**
	 * Get a Contact from a contact id.
	 *
	 * @param contactId the contact id
	 * @return the Contact or NULL if not found
	 */
	Contact * operator[](const std::string & contactId) const;
	
	/**
	 * Compare two groups.
	 *
	 * @param contactGroup group to compare
	 * @return true if equal
	 */
	bool operator==(const ContactGroup & contactGroup) const;

	std::string serialize();

	bool unserialize(const std::string & data);

private:

	/** Defines the vector of Contact. */
	typedef List<Contact *> Contacts;

	/** List of Contact associated with the ContactGroup. */
	Contacts _contactList;

	/** Name of the ContactGroup. */
	std::string _groupName;
};

#endif	//CONTACTGROUP_H
