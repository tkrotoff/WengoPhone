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

#include "WengoAccountFileStorage.h"

#include "WengoAccount.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/File.h>
#include <util/Logger.h>

#include <string>
using namespace std;

static const std::string WENGOACCOUNT_FILENAME = "user.config";

WengoAccountFileStorage::WengoAccountFileStorage(WengoAccount & wengoAccount)
	: WengoAccountStorage(wengoAccount) {
}

WengoAccountFileStorage::~WengoAccountFileStorage() {
}

bool WengoAccountFileStorage::load(const std::string & url) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	FileReader file(url + WENGOACCOUNT_FILENAME);

	if (file.open()) {
		string data = file.read();
		file.close();

		if (!_wengoAccount.unserialize(data)) {
			return false;
		}

		return true;
	}

	return false;
}

bool WengoAccountFileStorage::save(const std::string & url) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	FileWriter file(url + WENGOACCOUNT_FILENAME);

	std::string data = _wengoAccount.serialize();
	if (data.empty()) {
		return false;
	}

	file.write(data);
	file.close();
	return true;
}
