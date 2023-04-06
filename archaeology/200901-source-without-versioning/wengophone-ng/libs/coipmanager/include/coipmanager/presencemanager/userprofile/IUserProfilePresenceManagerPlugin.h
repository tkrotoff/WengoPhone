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

#ifndef OWIUSERPROFILEPRESENCEMANAGERPLUGIN_H
#define OWIUSERPROFILEPRESENCEMANAGERPLUGIN_H

#include <coipmanager/coipmanagerdll.h>
#include <coipmanager_base/EnumPresenceState.h>

#include <util/Interface.h>

#include <string>

class OWPicture;

/**
 * Interface for UserProfile Presence Management.
 *
 * @author Philippe Bernery
 */
class IUserProfilePresenceManagerPlugin : Interface {
public:

	/**
	 * Sets presence state on an UserProfile: online, offline, under the shower...
	 *
	 * @param state new presence state
	 * @param note personnalized status string ("I'm under the shower"),
	 *        used only with PresenceUserDefined
	 */
	virtual void setPresenceState(EnumPresenceState::PresenceState state, const std::string & note = String::null) = 0;

	/**
	 * Sets alias on an UserProfile (e.g: 'Joe... at the beach!').
	 *
	 * @param alias the desired alias.
	 */
	virtual void setAlias(const std::string & alias) = 0;

	/**
	 * Sets icon of UserProfile.
	 *
 	 * @param picture the desired icon
	 */
	virtual void setIcon(const OWPicture & picture) = 0;
};

#include <coipmanager/coipmanagerplugindll.h>
/**
 * Gets the IUserProfilePresenceManagerPlugin instance.
 *
 * Used for loading the DLL at runtime.
 * It's easier to load at runtime a C function rather than a C++ method.
 */
extern "C" COIPMANAGER_PLUGIN_API IUserProfilePresenceManagerPlugin * coip_userprofilepresence_plugin_init(CoIpManager & coIpManager);

#endif	//OWIUSERPROFILEPRESENCEMANAGERPLUGIN_H
