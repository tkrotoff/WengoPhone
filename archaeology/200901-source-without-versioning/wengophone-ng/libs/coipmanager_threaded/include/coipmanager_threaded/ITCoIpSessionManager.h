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

#ifndef OWITCOIPSESSIONMANAGER_H
#define OWITCOIPSESSIONMANAGER_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <coipmanager_base/contact/ContactList.h>

#include <QtCore/QObject>

class Account;
class ICoIpSessionManager;
class TCoIpManager;

/**
 * Interface for Threaded Session Managers.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API ITCoIpSessionManager : public QObject {
	Q_OBJECT
public:

	/**
	 * Default constructor for all managers.
	 *
	 * @param coIpManager link to the CoIpManager
	 */
	ITCoIpSessionManager(TCoIpManager & tCoIpManager);

	virtual ~ITCoIpSessionManager();

	/**
	 * @see ICoIpSessionManager::canCreateSession(const ContactList &)
	 */
	bool canCreateTSession(const ContactList & contactList) const;

	/**
	 * @see ICoIpSessionManager::canCreateSession(const Account &, const ContactList &)
	 */
	bool canCreateTSession(const Account & account, const ContactList & contactList) const;

protected:

	/**
	 * @return the ICoIpSessionManager pointer of the implementation.
	 */
	virtual ICoIpSessionManager * getICoIpSessionManager() const = 0;

	/** Link to TCoIpManager. */
	TCoIpManager & _tCoIpManager;

};

#endif //OWITCOIPSESSIONMANAGER_H
