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

#ifndef OWICONNECTMANAGER_H
#define OWICONNECTMANAGER_H

#include <coipmanager/ICoIpManagerPlugin.h>

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <string>

/**
 * Provides IConnectPlugin creation service.
 *
 * @author Philippe Bernery
 */
class IConnectManagerPlugin : public ICoIpManagerPlugin {
public:

	COIPMANAGER_API IConnectManagerPlugin(CoIpManager & coIpManager);

	COIPMANAGER_API virtual ~IConnectManagerPlugin();

	/**
	 * Creates an IConnectPlugin.
	 */
	virtual IConnectPlugin * createIConnectPlugin(const Account & account) = 0;
};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IConnectManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IConnectManagerPlugin * coip_connect_plugin_init(CoIpManager & coIpManager);

#endif	//OWICONNECTMANAGER_H
