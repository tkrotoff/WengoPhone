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

#include "Config.h"

#include <util/StringList.h>
#include <util/Logger.h>
#include <cutil/global.h>

const int Config::CONFIG_VERSION = 1;
const std::string Config::CONFIG_VERSION_KEY = "config.version";
const std::string Config::PROFILE_LAST_USED_ID_KEY = "profile.last_used_id";

Config::Config()
	: CoIpManagerConfig() {

	static const std::string empty(String::null);
	static const StringList emptyStringList;

	_keyDefaultValueMap[CONFIG_VERSION_KEY] = CONFIG_VERSION;
	set(CONFIG_VERSION_KEY, CONFIG_VERSION);

	_keyDefaultValueMap[PROFILE_LAST_USED_ID_KEY] = empty;
}

Config::~Config() {
}

int Config::getConfigVersion() const {
	return getIntegerKeyValue(CONFIG_VERSION_KEY);
}

std::string Config::getProfileLastUsedId() const {
	return getStringKeyValue(PROFILE_LAST_USED_ID_KEY);
}
