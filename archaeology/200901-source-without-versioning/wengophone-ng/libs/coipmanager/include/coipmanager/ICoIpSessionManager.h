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

#ifndef OWICOIPSESSIONMANAGER_H
#define OWICOIPSESSIONMANAGER_H

#include <coipmanager/ICoIpManager.h>

#include <coipmanager_base/contact/ContactList.h>

class Account;
class CoIpManager;
class ICoIpSessionManagerPlugin;

/**
 * Interface for Session Managers.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API ICoIpSessionManager : public ICoIpManager {
public:

	/**
	 * Default constructor for all managers.
	 *
	 * @param coIpManager link to the CoIpManager
	 */
	ICoIpSessionManager(CoIpManager & coIpManager);

	virtual ~ICoIpSessionManager();

	/**
	 * Checks if a session can created.
	 * @return true if a session can be created.
	 */
	bool canCreateSession(const ContactList & contactList) const;

	/**
	 * Checks if a session can created with given parameter.
	 * @return true if a session can be created.
	 */
	bool canCreateSession(const Account & account, const ContactList & contactList) const;

	/**
	 * Gets the ICoIpManagerPlugin available for the given parameter.
	 * 
	 * @return a pointer to the found ICoIpSessionManagerPlugin or NULL if not found.
	 */
	virtual ICoIpSessionManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

};

#endif //OWICOIPSESSIONMANAGER_H
