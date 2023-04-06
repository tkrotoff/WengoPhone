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

#include "PhApiInitPlugin.h"

#include <coipmanager/CoIpManager.h>

#include <util/File.h>
#include <util/Logger.h>

#include <PhApiWrapper.h>

ICoIpManagerInitPlugin * coip_init_plugin_init(CoIpManager & coIpManager) {
	return new PhApiInitPlugin(coIpManager);
}

PhApiInitPlugin::PhApiInitPlugin(CoIpManager & coIpManager)
	: ICoIpManagerInitPlugin(coIpManager) {

	// Creates PhApiWrapper instance
	PhApiWrapper::getInstance();

	// Sets path to phapi plugins
	// Look for a "phapi-plugins" directory in all coip plugin path 
	StringList pathList = _coIpManager.getCoIpManagerConfig().getCoIpPluginsPath();
	StringList::const_iterator it = pathList.end();
	std::string result;
	for (it = pathList.begin();
		it != pathList.end();
		++it) {
		result = File::normalize((*it) + "phapi-plugins/");
		if (File::exists(result)) {
			break;
		}
	}

	if (it != pathList.end()) {
		PhApiWrapper::getInstance()->setPluginPath(result);
	} else {
		LOG_ERROR("no phapi-plugins path found");
	}
}

PhApiInitPlugin::~PhApiInitPlugin() {
}

void PhApiInitPlugin::initialize() {
	PhApiWrapper::getInstance()->init();
}

void PhApiInitPlugin::uninitialize() {
	PhApiWrapper::getInstance()->terminate();
}
