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

#include "WengoAccountXMLLayer.h"

#include "WengoAccount.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/File.h>
#include <util/Logger.h>

#include <string>
using namespace std;

WengoAccountXMLLayer::WengoAccountXMLLayer(WengoAccount & wengoAccount)
: WengoAccountDataLayer(wengoAccount) {
}

WengoAccountXMLLayer::~WengoAccountXMLLayer() {
}

bool WengoAccountXMLLayer::load() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	FileReader file(config.getConfigDir() + "user.config");

	if (file.open()) {
		string data = file.read();
		file.close();

		if (!_wengoAccount.unserialize(data)) {
			return false;
		}

		LOG_DEBUG("file user.config loaded");
		return true;
	}

	return false;
}

bool WengoAccountXMLLayer::save() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	FileWriter file(config.getConfigDir() + "user.config");

	std::string data = _wengoAccount.serialize();
	if (data.empty()) {
		return false;
	}
	file.write(data);
	file.close();
	return true;
}
