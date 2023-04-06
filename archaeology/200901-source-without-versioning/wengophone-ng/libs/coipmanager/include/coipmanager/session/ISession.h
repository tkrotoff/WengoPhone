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

#ifndef OWISESSION_H
#define OWISESSION_H

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/imcontact/IMContactList.h>

#include <coipmanager/CoIpModule.h>

#include <QtCore/QMutex>

/**
 * Base class for Session implementations.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ISession : public CoIpModule {
	Q_OBJECT
public:

	ISession(CoIpManager & coIpManager);

	ISession(const ISession & iSession);

	virtual ~ISession();

	/**
	 * Sets the Account to use.
	 */
	virtual void setAccount(const Account & account);

	/**
	 * Gets the used Account.
	 */
	Account getAccount() const;

	/**
	 * Adds an IMContact to the ISession.
	 * If needed, the IMContact is added remotely.
	 *
	 * Thread safe.
	 */
	virtual void addIMContact(const IMContact & imContact);

	/**
	 * Removes an IMContact from the ISession.
	 * If needed, the IMContact is removed remotely.
	 *
	 * Thread safe.
	 *
	 * No effect if contactId not found.
	 */
	virtual void removeIMContact(const IMContact & imContact);

	/**
	 * Adds an IMContact to the ISession.
	 * Doesn't add it remotely.
	 *
	 * Thread safe.
	 */
	virtual void addIMContactLocally(const IMContact & imContact);

	/**
	 * Removes an IMContact from the ISession.
	 * Doesn't remove it locally.
	 *
	 * Thread safe.
	 *
	 * No effect if contactId not found.
	 */
	virtual void removeIMContactLocally(const IMContact & imContact);

	/**
	 * Sets the list of IMContact.
	 */
	virtual void setIMContactList(const IMContactList & imContactList);

	/**
	 * Gets the list of IMContact.
	 */
	IMContactList getIMContactList() const;

Q_SIGNALS:

	/**
	 * Emitted when an IMContact has been added in this ISession.
	 *
	 * @param imContact the added IMContact
	 */
	void contactAddedSignal(IMContact imContact);

	/**
	 * Emitted when an IMContact has been removed.
	 *
	 * @param imContact the removed IMContact
	 */
	void contactRemovedSignal(IMContact imContact);

protected:

	/** Mutex for Thread safe operation. */
	mutable QMutex * _mutex;

	/** List of Contact. */
	IMContactList _imContactList;

	/** Account to use. */
	Account _account;
};

#endif	//OWISESSION_H
