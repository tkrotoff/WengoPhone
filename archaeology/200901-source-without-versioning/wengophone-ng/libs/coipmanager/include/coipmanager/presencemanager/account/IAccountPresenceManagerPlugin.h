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

#ifndef OWIACCOUNTPRESENCEMANAGERPLUGIN_H
#define OWIACCOUNTPRESENCEMANAGERPLUGIN_H

#include <coipmanager_base/EnumPresenceState.h>
#include <coipmanager/ICoIpManagerPlugin.h>

#include <util/String.h>

class Account;
class OWPicture;

/**
 * Interface for Account Presence Management.
 *
 * @author Philippe Bernery
 */
class IAccountPresenceManagerPlugin : public ICoIpManagerPlugin {
public:

	COIPMANAGER_API IAccountPresenceManagerPlugin(CoIpManager & coIpManager);

	COIPMANAGER_API virtual ~IAccountPresenceManagerPlugin();

	/**
	 * Sets presence state on an Account: online, offline, under the shower...
	 *
	 * @param account Account whose presence state must be set.
	 * @param state new presence state
	 * @param note personnalized status string ("I'm under the shower"),
	 *        used only with PresenceUserDefined
	 */
	virtual void setPresenceState(const Account & account,
		EnumPresenceState::PresenceState state, const std::string & note = String::null) = 0;

	/**
	 * Sets alias on an Account (e.g: 'Joe... at the beach!').
	 *
	 * @param account Account whose alias must be set.
	 * @param alias the desired alias.
	 */
	virtual void setAlias(const Account & account, const std::string & alias) = 0;

	/**
	 * Sets icon of Account.
	 *
 	 * @param account Account whose presence state must be set.
	 * @param picture the desired icon
	 */
	virtual void setIcon(const Account & account, const OWPicture & picture) = 0;
};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IAccountPresenceManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IAccountPresenceManagerPlugin * coip_accountpresence_plugin_init(CoIpManager & coIpManager);

#endif	//OWIACCOUNTPRESENCEMANAGERPLUGIN_H
