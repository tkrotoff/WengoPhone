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

#include "ConfigManagerFileStorage.h"

#include <model/config/ConfigManager.h>
#include <model/config/ConfigXMLSerializer.h>

#include <util/File.h>
#include <util/Logger.h>

using namespace std;

static const std::string CONFIG_FILENAME = "config.xml";

ConfigManagerFileStorage::ConfigManagerFileStorage(ConfigManager & configManager)
	: ConfigManagerStorage(configManager) {
}

ConfigManagerFileStorage::~ConfigManagerFileStorage() {
}

bool ConfigManagerFileStorage::load(const std::string & url) {
	FileReader file(url + CONFIG_FILENAME);
	if (file.open()) {
		string data = file.read();
		file.close();

		ConfigXMLSerializer serializer(_configManager.getCurrentConfig());
		serializer.unserialize(data);
		return true;
	}

	return false;
}

bool ConfigManagerFileStorage::save(const std::string & url) {
	FileWriter file(url + CONFIG_FILENAME);
	ConfigXMLSerializer serializer(_configManager.getCurrentConfig());

	file.write(serializer.serialize());
	file.close();
	return true;
}
