/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "ConfigManager.h"

#include "Config.h"

using namespace std;

ConfigManager::ConfigManager() {
	const string defaultConfig = "default";

	_configList[defaultConfig] = new Config(defaultConfig);
	setCurrentConfig(defaultConfig);
}

ConfigManager::~ConfigManager() {
	for (ConfigList::const_iterator it = _configList.begin()
		; it != _configList.end() ; it++) {
		delete (*it).second;	
	}
}

Config & ConfigManager::getCurrentConfig() const {
	return *_currentConfig;
}

void ConfigManager::setCurrentConfig(const std::string & configName) {
	ConfigList::const_iterator it = _configList.find(configName);

	if (it != _configList.end()) {
		_currentConfig = (*it).second;
	}
}

StringList ConfigManager::getConfigList() const {
	StringList result;

	for (ConfigList::const_iterator it = _configList.begin()
		; it != _configList.end() ; it++) {
		result += (*it).first;
	}

	return result;
}

Config * ConfigManager::getConfig(const std::string & configName) const {
	ConfigList::const_iterator it = _configList.find(configName);

	if (it != _configList.end()) {
		return (*it).second;
	} else {
		return NULL;	
	}
}

void ConfigManager::addConfig(Config * config) {
	if (config) {
		_configList[config->getName()] = config;	
	}	
}
