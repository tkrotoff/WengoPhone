/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef OWISMSSESSIONMANAGERPLUGIN_H
#define OWISMSSESSIONMANAGERPLUGIN_H

#include <coipmanager/ICoIpSessionManagerPlugin.h>

class ISMSSessionPlugin;

/**
 * Interface for SMS Session Manager implementation.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API ISMSSessionManagerPlugin : public ICoIpSessionManagerPlugin {
public:

	ISMSSessionManagerPlugin(CoIpManager & coIpManager);

	virtual ~ISMSSessionManagerPlugin();

	/**
	 * Creates an ISMSSession.
	 */
	virtual ISMSSessionPlugin * createISMSSessionPlugin() = 0;

};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the ISMSSessionManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API ISMSSessionManagerPlugin * coip_smssession_plugin_init(CoIpManager & coIpManager);

#endif //OWISMSSESSIONMANAGERPLUGIN_H
