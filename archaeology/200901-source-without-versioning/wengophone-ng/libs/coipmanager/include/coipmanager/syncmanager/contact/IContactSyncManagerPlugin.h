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

#ifndef OWICONTACTSYNCMANAGER_H
#define OWICONTACTSYNCMANAGER_H

#include <coipmanager_base/contact/Contact.h>

#include <coipmanager/syncmanager/ISyncManagerPlugin.h>

/**
 * Interface for Contact data synchronization implementations.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IContactSyncManagerPlugin : public ISyncManagerPlugin {
	Q_OBJECT
public:

	IContactSyncManagerPlugin(CoIpManager & coIpManager);

	virtual ~IContactSyncManagerPlugin();

	/**
	 * Adds a Contact to external lib.
	 *
	 * @param contact the Contact to add.
	 */
	virtual void add(const Contact & contact) = 0;

	/**
	 * Removes a Contact from external lib.
	 *
	 * @param contact the Contact to remove.
	 */
	virtual void remove(const Contact & contact) = 0;

	/**
	 * Updates a Contact to external lib.
	 *
	 * @param contact the Contact to update.
	 */
	virtual void update(const Contact & contact) = 0;

Q_SIGNALS:

	/**
	 * Emitted when a Contact has been externally added.
	 */
	void contactAddedSignal(Contact contact);

	/**
	 * Emitted when a Contact has been externally removed.
	 */
	void contactRemovedSignal(Contact contact);

	/**
	 * Emitted when a Contact has been externally updated.
	 */
	void contactUpdatedSignal(Contact contact);

};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IContactSyncManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IContactSyncManagerPlugin * coip_contactsync_plugin_init(CoIpManager & coIpManager);

#endif	//OWICONTACTSYNCMANAGER_H
