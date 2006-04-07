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

#include <settings/AutomaticSettings.h>

#include <util/Logger.h>

AutomaticSettings::AutomaticSettings() {
}

AutomaticSettings::~AutomaticSettings() {
}

StringList AutomaticSettings::getAllKeys() const {
	StringList tmp;
	for (Keys::const_iterator it = _keyDefaultValueMap.begin(); it != _keyDefaultValueMap.end(); ++it) {
		tmp += it->first;
	}
	return tmp;
}

boost::any AutomaticSettings::getAny(const std::string & key) const {
	return Settings::getAny(key, getDefaultValue(key));
}

boost::any AutomaticSettings::getDefaultValue(const std::string & key) const {
	Keys::const_iterator it = _keyDefaultValueMap.find(key);
	if (it == _keyDefaultValueMap.end()) {
		LOG_FATAL("key=" + key + " not found, add it inside the AutomaticSettings constructor");
	}

	KeyValue keyValue = it->second;
	boost::any defaultValue = keyValue.value;
	if (defaultValue.empty()) {
		LOG_FATAL("default value for key=" + key + " not defined, add it inside the AutomaticSettings constructor");
	}

	return defaultValue;
}

bool AutomaticSettings::getBooleanKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isBoolean(value)) {
		LOG_FATAL("value for key=" + key + " is not a boolean");
	}

	return boost::any_cast<bool>(value);
}

int AutomaticSettings::getIntegerKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isInteger(value)) {
		LOG_FATAL("value for key=" + key + " is not an integer");
	}

	return boost::any_cast<int>(value);
}

std::string AutomaticSettings::getStringKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isString(value)) {
		LOG_FATAL("value for key=" + key + " is not a string");
	}

	return boost::any_cast<std::string>(value);
}

StringList AutomaticSettings::getStringListKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isStringList(value)) {
		LOG_FATAL("value for key=" + key + " is not a string list");
	}

	return boost::any_cast<StringList>(value);
}
