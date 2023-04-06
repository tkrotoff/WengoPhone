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

#include "PurpleInitPlugin.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <purplewrapper/PurpleWrapper.h>

ICoIpManagerInitPlugin * coip_init_plugin_init(CoIpManager & coIpManager) {
	return new PurpleInitPlugin(coIpManager);
}

PurpleInitPlugin::PurpleInitPlugin(CoIpManager & coIpManager)
	: ICoIpManagerInitPlugin(coIpManager) {

	// Creates PurpleWrapper instance.
	PurpleWrapper::getInstance();
}

PurpleInitPlugin::~PurpleInitPlugin() {
}

void PurpleInitPlugin::initialize() {
	// Sets path to gaim config.
	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	UserProfileFileStorage userProfileFileStorage(_coIpManager.getCoIpManagerConfig().getConfigPath());
	PurpleWrapper::getInstance().setConfigPath(userProfileFileStorage.getProfilePath(userProfile.getUUID()));

	PurpleWrapper::getInstance().initialize();
}

void PurpleInitPlugin::uninitialize() {
	PurpleWrapper::getInstance().uninitialize();
}
