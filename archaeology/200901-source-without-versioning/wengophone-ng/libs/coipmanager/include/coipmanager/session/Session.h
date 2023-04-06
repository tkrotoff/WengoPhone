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

#ifndef OWSESSION_H
#define OWSESSION_H

#include <coipmanager/CoIpModule.h>
#include <coipmanager/session/EnumSessionError.h>

#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/imcontact/IMContactList.h>

#include <QtCore/QMutex>

class Account;
class ISession;

/**
 * Base class for all kind of Session.
 *
 * This base class defines the constructor and some members to add/remove
 * Contact. Here we manipulate Contact objects and not only contactIds as
 * a Session could contain Contact that are not in the ContactList.
 *
 * When starting a Session, if setAccountToUse is not called, the right
 * Account to use is choosen by taking the first Account usable with all
 * Contacts available in the Session.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API Session : public CoIpModule {
	Q_OBJECT
public:

	Session(CoIpManager & coIpManager);

	Session(const Session & session);

	virtual ~Session();

	/**
	 * Adds a Contact to the Session.
	 *
	 * @return SessionErrorInvalidParameter error if the Contact cannot be added (no Session implementation could
	 * be created when starting the Session).
	 */
	virtual EnumSessionError::SessionError addContact(const Contact & contact);

	/**
	 * Removes a Contact from the Session.
	 *
	 * No effect if contactId not found.
	 */
	virtual void removeContact(const std::string & contactId);

	/**
	 * Sets the list of Contact.
	 *
	 * @return SessionErrorInvalidParameter if the Contact cannot be added (no Session implementation could
	 * be created when starting the Session).
	 */
	virtual EnumSessionError::SessionError setContactList(const ContactList & contactList);

	/**
	 * Gets the list of Contact inside the Session.
	 */
	ContactList getContactList() const;

	/**
	 * Force the Account to use for launching the session.
	 * If not called, the Account will be automatically choosed following rules described in
	 * Session class description.
	 *
	 * Adding a Contact (with addContact) will override this selection.
	 *
	 * @return SessionErrorInvalidParameter if no Session implementation found for set
	 * set Contacts and given Account.
	 */
	virtual EnumSessionError::SessionError setAccountToUse(const std::string & accountId);

	/**
	 * @return the duration of the Session.
	 */
	//int getDuration() const;

	/**
	 * @return the used Account of the Session.
	 */
	Account * getAccount() const;

Q_SIGNALS:

	/**
	 * Emitted when a Contact has been added in this Session.
	 *
	 * @param contact the added Contact
	 */
	void contactAddedSignal(Contact contact);

	/**
	 * Emitted when a Contact has been removed.
	 *
	 * @param sender this class
	 * @param contact the removed Contact
	 */
	void contactRemovedSignal(Contact contact);

protected:

	/**
	 * Checks if the given Account can be used to start a Session
	 * with Contacts given in ContactList.
	 *
	 * @return true if Account usable, false otherwise.
	 */
	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const = 0;

	/**
	 * @see ISession::contactAddedSignal
	 */
	void contactAddedSlot(IMContact imContact);

	/**
	 * @see ISession::contactRemovedSignal
	 */
	void contactRemovedSlot(IMContact imContact);

	/**
	 * Gets the first Account usable with the given ContactList.
	 */
	std::string getFirstUsableAccount(const ContactList & contactList) const;

	/**
	 * Finds the Contact inside this Session that has the given IMContact.
	 */
	Contact getContactThatOwns(const IMContact & imContact) const;

	/** Mutex for Thread safe operation. */
	mutable QMutex * _mutex;

	/** List of Contact. */
	ContactList _contactList;

	/**
	 * Account to use. It is computed in addContact and setContactList. It can be
	 * manually set with setAccountToUse.
	 */
	std::string _accountToUse;

};

#endif	//OWSESSION_H
