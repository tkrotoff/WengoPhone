/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "StartupSettingListener.h"

#include "Config.h"
#include "ConfigManager.h"

#include <system/Startup.h>

#include <util/Logger.h>
#include <util/Path.h>

#include <iostream>

StartupSettingListener::StartupSettingListener() {
	_startup = new Startup("WengoPhone", Path::getApplicationDirPath() + "qtwengophone.exe");
	ConfigManager::getInstance().getCurrentConfig().valueChangedEvent += boost::bind(&StartupSettingListener::startupSettingChanged, this, _1, _2);
}

StartupSettingListener::~StartupSettingListener() {
	if (_startup) {
		delete _startup;
	}
}

void StartupSettingListener::startupSettingChanged(Settings & sender, const std::string & key) const {
	if (key == Config::GENERAL_AUTOSTART_KEY) {
		_startup->setStartup(ConfigManager::getInstance().getCurrentConfig().getGeneralAutoStart());
	}
}
