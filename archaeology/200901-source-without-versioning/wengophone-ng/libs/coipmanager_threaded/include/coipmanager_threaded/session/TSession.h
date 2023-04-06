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

#ifndef OWTSESSION_H
#define OWTSESSION_H

#include <coipmanager_threaded/TCoIpModule.h>

#include <coipmanager/session/Session.h>

/**
 * Threaded Session.
 *
 * @see Session
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TSession : public TCoIpModule {
	Q_OBJECT
public:

	TSession(TCoIpManager & tCoIpManager, Session * session);

	virtual ~TSession();

	/**
	 * @see Session::addContact()
	 */
	EnumSessionError::SessionError addContact(const Contact & contact);

	/**
	 * @see Session::removeContact()
	 */
	void removeContact(const std::string & contactId);

	/**
	 * @see Session::setContactList()
	 */
	EnumSessionError::SessionError setContactList(const ContactList & contactList);

	/**
	 * @see Session::getAccount()
	 */
	Account * getAccount() const;

	/**
	 * @see Session::getContactList()
	 */
	ContactList getContactList() const;

Q_SIGNALS:

	/**
	 * @see Session::contactAddedSignal
	 */
	void contactAddedSignal(Contact contact);

	/**
	 * @see Session::contactRemovedSignal
	 */
	void contactRemovedSignal(Contact contact);

protected:

	/**
	 * Code factorization.
	 */
	Session * getSession() const;
};

#endif	//OWTSESSION_H
