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

#include "ConfigXMLSerializer.h"

#include "Config.h"

#include <tinyxml.h>

ConfigXMLSerializer::ConfigXMLSerializer(Config & config) 
: SettingsXMLSerializer(config) {
}

bool ConfigXMLSerializer::unserialize(const std::string & data) {
	bool result = SettingsXMLSerializer::unserialize(data);

	if (result) {
		TiXmlDocument doc;

		doc.Parse(data.c_str());

		TiXmlHandle docHandle(&doc);
		TiXmlHandle settings = docHandle.FirstChild("settings");

		// Looking for the config version.
		// If the config version does not exist, it means that the config is older
		// than version 4. We force it to put at version 0 to allow easy detection
		// of WengoPhone in ConfigImporter.
		TiXmlNode * version = settings.FirstChild("config.version").Node();
		if (!version) {
			_settings.set(Config::CONFIG_VERSION_KEY, 0);
		}
		////

	}

	return result;
}
