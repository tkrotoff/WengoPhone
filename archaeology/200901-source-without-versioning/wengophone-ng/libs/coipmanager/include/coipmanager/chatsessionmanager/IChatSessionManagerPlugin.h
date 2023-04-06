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

#ifndef OWICHATSESSIONMANAGERPLUGIN_H
#define OWICHATSESSIONMANAGERPLUGIN_H

#include <coipmanager/ICoIpSessionManagerPlugin.h>

class IChatSessionPlugin;

/**
 * Interface for Chat Session Manager implementation.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API IChatSessionManagerPlugin : public ICoIpSessionManagerPlugin {
	Q_OBJECT
public:

	IChatSessionManagerPlugin(CoIpManager & coIpManager);

	virtual ~IChatSessionManagerPlugin();

	/**
	 * Creates an IChatSession.
	 */
	virtual IChatSessionPlugin * createIChatSessionPlugin() = 0;

Q_SIGNALS:

	/**
	 * Emitted when a new IChatSession is created.
	 *
	 * This event is emitted when someone tries to contact the user.
	 *
	 * @param chatSession pointer to the newly created IChatSession
	 *        Receiver is responsible for deleting the Session
	 */
	void newIChatSessionPluginCreatedSignal(IChatSessionPlugin * iChatSession);
};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IChatSessionManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IChatSessionManagerPlugin * coip_chatsession_plugin_init(CoIpManager & coIpManager);

#endif	//OWICHATSESSIONMANAGERPLUGIN_H
