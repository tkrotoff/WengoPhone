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

#ifndef OWICOIPSESSIONMANAGERPLUGIN_H
#define OWICOIPSESSIONMANAGERPLUGIN_H

#include <coipmanager/ICoIpManagerPlugin.h>

#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/imcontact/IMContactList.h>

class Account;

/**
 * Interface for managers implementation that can create Session.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API ICoIpSessionManagerPlugin : public ICoIpManagerPlugin {
public:

	/**
	 * Default constructor for all managers.
	 *
	 * @param coIpManager link to the CoIpManager
	 */
	ICoIpSessionManagerPlugin(CoIpManager & coIpManager);

	virtual ~ICoIpSessionManagerPlugin();

	/**
	 * Checks if a Session can be created with given parameters.
	 *
	 * @return true if an ISession can be created.
	 */
	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const = 0;

	/**
	 * @return the list of IMContact that will be used to make the Session.
	 */
	virtual IMContactList getValidIMContacts(const Account & account, const ContactList & contactList) const = 0;

};

#endif //OWICOIPSESSIONMANAGERPLUGIN_H
