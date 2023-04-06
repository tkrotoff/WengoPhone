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

#ifndef OWIIMCONTACTSYNCMANAGER_H
#define OWIIMCONTACTSYNCMANAGER_H

#include <coipmanager_base/imcontact/IMContact.h>

#include <coipmanager/syncmanager/ISyncManagerPlugin.h>

/**
 * Interface for IMContact data synchronization implementations.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IIMContactSyncManagerPlugin : public ISyncManagerPlugin {
	Q_OBJECT
public:

	IIMContactSyncManagerPlugin(CoIpManager & coIpManager);

	virtual ~IIMContactSyncManagerPlugin();

	/**
	 * Adds an IMContact to external lib.
	 *
	 * @param imContact the IMContact to add.
	 */
	virtual void add(const IMContact & imContact) = 0;

	/**
	 * Removes an IMContact from external lib.
	 *
	 * @param imContact the IMContact to remove.
	 */
	virtual void remove(const IMContact & imContact) = 0;

	/**
	 * Updates an IMContact to external lib.
	 *
	 * @param imContact the IMContact to update.
	 */
	virtual void update(const IMContact & imContact) = 0;

	/**
	 * Emission methods.
	 */
	void emitIMContactAddedSignal(IMContact imContact);

	void emitIMContactRemovedSignal(IMContact imContact);

	void emitIMContactUpdatedSignal(IMContact imContact);

Q_SIGNALS:

	/**
	 * Emitted when an IMContact has been externally added.
	 */
	void imContactAddedSignal(IMContact imContact);

	/**
	 * Emitted when an IMContact has been externally removed.
	 */
	void imContactRemovedSignal(IMContact imContact);

	/**
	 * Emitted when an IMContact has been externally updated.
	 */
	void imContactUpdatedSignal(IMContact imContact);

};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IIMContactSyncManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IIMContactSyncManagerPlugin * coip_imcontactsync_plugin_init(CoIpManager & coIpManager);

#endif	//OWIIMCONTACTSYNCMANAGER_H
