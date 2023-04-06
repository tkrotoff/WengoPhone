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

#ifndef OWICONTACTPRESENCEMANAGERPLUGIN_H
#define OWICONTACTPRESENCEMANAGERPLUGIN_H

#include <coipmanager/ICoIpManagerPlugin.h>
#include <coipmanager_base/imcontact/IMContact.h>

#include <QtCore/QObject>

/**
 * Interface for Contact Presence Management.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IContactPresenceManagerPlugin : public ICoIpManagerPlugin {
	Q_OBJECT
public:

	IContactPresenceManagerPlugin(CoIpManager & coIpManager);

	virtual ~IContactPresenceManagerPlugin();

	/**
	 * Avoid a contact to see my Presence.
	 *
	 * @param imContact the IMContact to block
	 */
	virtual void blockContact(const IMContact & imContact) = 0;

	/**
	 * Authorizes a contact to see my Presence.
	 *
	 * @param imContact the IMContact to unblock
	 */
	virtual void unblockContact(const IMContact & imContact) = 0;

	/**
	 * Authorizes a contact to add me in his contact list.
	 *
	 * @param imContact the IMContact to authorize
 	 * @param authorized TRUE if we agree
	 * @param message message for IMContact
	 */
	virtual void authorizeContact(const IMContact & imContact,
		bool authorized, const std::string & message) = 0;

	/**
	 * Called by CoIpManager when an IMContact has been added.
	 */
	virtual void imContactAdded(const IMContact & imContact);

	/**
	 * Called by CoIpManager when an IMContact has been removed.
	 */
	virtual void imContactRemoved(const IMContact & imContact);

	/**
	 * @see authorizationRequested.
	 */
	void emitAuthorizationRequested(IMContact imContact, const std::string & message);

Q_SIGNALS:

	/**
	 * Emitted when an IMContact change its presence state.
	 */
	void imContactUpdated(IMContact imContact);

	/**
	 * Emitted when a contact wants to add us in his buddy list.
	 */
	void authorizationRequested(IMContact imContact, const std::string & message);

};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IContactPresenceManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IContactPresenceManagerPlugin * coip_contactpresence_plugin_init(CoIpManager & coIpManager);

#endif	//OWICONTACTPRESENCEMANAGERPLUGIN_H
