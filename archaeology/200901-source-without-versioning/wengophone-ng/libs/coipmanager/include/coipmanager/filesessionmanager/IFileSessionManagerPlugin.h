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

#ifndef OWIFILESESSIONMANAGERPLUGIN_H
#define OWIFILESESSIONMANAGERPLUGIN_H

#include <coipmanager/ICoIpSessionManagerPlugin.h>

class IFileSessionManagerPlugin;
class IReceiveFileSessionPlugin;
class ISendFileSessionPlugin;
class UserProfile;

/**
 * File Session Manager.
 *
 * Used to manage classes related to File Transfer.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IFileSessionManagerPlugin : public ICoIpSessionManagerPlugin {
	Q_OBJECT
public:

	IFileSessionManagerPlugin(CoIpManager & coIpManager);

	virtual ~IFileSessionManagerPlugin();

	/**
	 * Creates an ISendFileSession in FileSessionModeSend mode.
	 */
	virtual ISendFileSessionPlugin * createISendFileSessionPlugin() = 0;

Q_SIGNALS:

	/**
	 * Emitted when a new IReceiveFileSession is created.
	 *
	 * @param sender this class
	 * @param fileSession pointer to the newly created ISendFileSession.
	 * Here the ISendFileSession is a pointer because the received of the event is
	 * responsible of deleting of this object.
	 */
	void newIReceiveFileSessionPluginCreatedSignal(IReceiveFileSessionPlugin * iReceiveFileSession);

	/**
	 * Emitted when the implementation needs to be upgraded.
	 *
	 * This happends when the peer implementation version is newer than this one.
	 */
	void needsUpgradeSignal();

	/**
	 * Emitted when the peer implementation needs to be upgraded.
	 *
	 * This happends when this implementation version is newer than the peer one.
	 */
	void peerNeedsUpgradeSignal(IMContact imContact);

};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IFileSessionManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IFileSessionManagerPlugin * coip_filesession_plugin_init(CoIpManager & coIpManager);

#endif	//OWIFILESESSIONMANAGERPLUGIN_H
