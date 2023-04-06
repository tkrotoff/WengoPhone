/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWCONTACTLIST_H
#define OWCONTACTLIST_H

#include <coipmanager_base/contact/Contact.h>

#include <list>

/**
 * Contact list.
 *
 * Contact can be ordered manually, order will be kept.
 *
 * @author Philippe Bernery
 */
typedef std::list<Contact> ContactList;

class COIPMANAGER_BASE_API ContactListHelper {
public:

	/**
	 * Gets the Contact identified by the given UUID.
	 *
	 * @return a copy of the Contact, NULL otherwise.
	 */
	static Contact * getCopyOfContact(const ContactList & contactList, const std::string & uuid);

	/**
	 * Find the first that has the given IMContact.
	 *
	 * @param imContactId the id of the IMContact to find
	 * @return a copy of the found contact, NULL otherwise
	 */
	static Contact * getCopyOfFirstContactThatOwns(const ContactList & contactList, const std::string & imContactId);

	/**
	 * Gets the first Contact that has an equivalent IMContact, meaning
	 * contactId/accountType is tested instead of IMContactId.
	 *
	 * @return a clone of the found Account, NULL otherwhise
	 */
	static Contact * getCopyOfFirstContactThatOwns(const ContactList & contactList, const IMContact & imContact);

	/**
	 * Looks for an equivalent IMContact in ContactList and copy it into
	 * imContact if found.
	 *
	 * @return true if found
	 */
	static bool getIMContact(const ContactList & contactList, IMContact & imContact);
};

#endif	//OWCONTACTLIST_H
