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

#ifndef OWCONTACTMANAGER_H
#define OWCONTACTMANAGER_H

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/imcontact/IMContact.h>

#include <coipmanager/coipmanagerdll.h>

#include <QtCore/QMutex>
#include <QtCore/QObject>

class Contact;

/**
 * Contact list manager.
 *
 * It accesses an ContactList in a thread-safe manner.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API ContactManager : public QObject {
	Q_OBJECT
public:

	ContactManager(ContactList & contactList);

	~ContactManager();

	/**
	 * Adds an Contact.
	 *
	 * Checks if the contact is not already in the list.
	 *
	 * @return true if actuallt added (the Contact was not present)
	 */
	bool add(const Contact & contact);

	/**
	 * Removes an Contact.
	 *
	 * @return true if actually removed (the Contact was present)
	 */
	bool remove(const std::string & contactId);

	/**
	 * Updates an Contact.
	 *
	 * @param section used to specify which part of the Contact has been updated.
	 *        This parameter is used for emitting contactUpdatedEvent and optimize
	 *        some part of the code. It is usually only specified by internal classes of
	 *        CoIpManager
	 * @return true if actually update (the Contact was updated)
	 */
	bool update(const Contact & contact,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);

Q_SIGNALS:

	/**
	 * Emitted when an Contact has been added.
	 */
	void contactAddedSignal(std::string contactId);

	/**
	 * Emitted when an Contact has been removed.
	 */
	void contactRemovedSignal(std::string contactId);

	/**
	 * Emitted when an Contact has been updated.
	 *
	 * @param contactId id of the updated Contact
	 * @param section updated section of the Contact
	 */
	void contactUpdatedSignal(std::string contactId,
		EnumUpdateSection::UpdateSection section);

	/**
	 * Emitted when an IMContact has been added to a Contact.
	 * Typically useful for internal modules (like synchronizers)
	 *
	 * @param contactId the Contact where the IMContact is from
	 * @param imContact the added IMContact
	 */
	void imContactAddedSignal(std::string contactId, IMContact imContact);

	/**
	 * Emitted when an IMContact has been removed from a Contact.
	 * Typically useful for internal modules (like synchronizers).
	 *
	 * @param contactId the Contact where the IMContact is from
	 * @param imContact the removed IMContact
	 */
	void imContactRemovedSignal(std::string contactId, IMContact imContact);

private:

	static ContactList::iterator findContact(ContactList & contactList, const Contact & contact);

	ContactList & _contactList;

	QMutex * _mutex;
};

#endif	//OWCONTACTMANAGER_H
