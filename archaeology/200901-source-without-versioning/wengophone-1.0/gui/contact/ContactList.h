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

#include "Contact.h"
#include "MainWindow.h"

#include <observer/Subject.h>
#include <singleton/Singleton.h>

#include <qstring.h>

#include <vector>

/**
 * Represents a list of Contact.
 *
 * Design Pattern Singleton
 * Design Pattern Observer
 * ContactList is observed by ContactListWidget in order for ContactListWidget
 * to be updated, this is usefull when a new Contact is added to ContactList
 * or when a Contact is removed from ContactList.
 * In the case of a Contact that is updated, the Contact will directy talk
 * with ContactListWidget.
 *
 * @author Tanguy Krotoff
 */
class ContactList : public Subject, public Singleton<ContactList> {
	friend class Singleton<ContactList>;
	friend class MainWindow;
public:

	/**
	 * Defines the vector of Contact.
	 */
	typedef std::vector<Contact *> Contacts;

	~ContactList();

	/**
	 * Appends the specified element to the end of the list.
	 *
	 * @param contact element to be appended to the ContactList
	 */
	void addContact(Contact & contact);

	/**
	 * Removes a single instance of the specified element from this list, if it is present.
	 *
	 * @param contact element to remove from the ContactList
	 * @return true if the collection contained the specified element
	 */
	bool removeContact(Contact & contact);

	/**
	 * If the Contact is present inside the ContactList or not.
	 *
	 * @return true if the Contact is contained inside the ContactList, false otherwise
	 */
	bool contains(const Contact & contact);

	/**
	 * Gets a Contact given its ID.
	 *
	 * If contactId is incorrect, this method will make an assert().
	 *
	 * @param contactId ID of the Contact
	 * @return the Contact that corresponds to the contactId
	 */
	const Contact & getContact(const ContactId & contactId) const;

	/**
	 * @see getContact()
	 */
	Contact & getContact(const ContactId & contactId);

	/**
	 * Gets a Contact given a phone number if present in the ContactList.
	 *
	 * @param phoneNumber phone number of the Contact
	 * @return the Contact if present in the ContactList, NULL otherwise
	 */
	Contact * getContact(const QString & phoneNumber);

	/**
	 * Gets the number of Contact inside the ContactList.
	 *
	 * @return size of the collection/vector
	 */
	unsigned int size() const;

	/**
	 * Permits to use ContactList as an array.
	 *
	 * <code>
	 * ContactList contactList;
	 * contactList.addContact(...);
	 * contactList.addContact(...);
	 * for (unsigned int i = 0; i  contactList.size(); i++) {
	 *     Contact & contact = contactList[i];
	 * }
	 * </code>
	 *
	 * @param i index inside the array
	 * @return the Contact that corresponds to the index i inside the ContactList
	 */
	const Contact & operator[] (unsigned int i) const;

	/**
	 * @see operator[]
	 */
	Contact & operator[] (unsigned int i);

	/**
	 * Resets the ContactList: erases all the Contact from the list.
	 *
	 * Does not erase the files on the hard disk.
	 */
	void reset();

protected:

	ContactList();

	/*ContactList(const ContactList & contactList)
	: Subject(), _contactList(contactList._contactList) {
	}*/

	ContactList(const ContactList &);
	ContactList & operator=(const ContactList &);

private:

	void eraseAll();

	/**
	 * List of Contact.
	 */
	Contacts _contactList;
};

#endif	//CONTACTLIST_H
