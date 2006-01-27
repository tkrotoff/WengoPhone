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

#include <Settings.h>

#include <Logger.h>

using namespace std;
using boost::any_cast;

const string Settings::DOT = ".";

Settings::Settings() {
}

StringList Settings::getAllKeys() const {
	StringList tmp;
	for (Keys::const_iterator it = _keyMap.begin(); it != _keyMap.end(); ++it) {
		tmp += it->first;
	}
	return tmp;
}

void Settings::remove(const std::string & key) {
	Keys::iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		_keyMap.erase(it);
	} else {
		//Erasing a group of key

		//Looking for the first key
		Keys::iterator it = _keyMap.begin();
		while ((it != _keyMap.end())
			&& (!(String(it->first).beginsWith(key)))) {
			it++;
		}

		//Erasing keys
		while ((it != _keyMap.end()) && (String(it->first).beginsWith(key))) {
			_keyMap.erase(it);
			it++;
		}
	}
}

bool Settings::contains(const std::string & key) {
	Keys::iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		return true;
	}
	return false;
}

void Settings::set(const std::string & key, const std::string & value) {
	_keyMap[key] = value;
}

void Settings::set(const std::string & key, bool value) {
	_keyMap[key] = value;
}

void Settings::set(const std::string & key, int value) {
	_keyMap[key] = value;
}

std::string Settings::get(const std::string & key, const std::string & defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isString(it->second)) {
			return any_cast<std::string>(it->second);
		} else {
			LOG_FATAL("The key doesn't match a string value");
		}
	}

	return defaultValue;
}

bool Settings::get(const std::string & key, bool defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isBoolean(it->second)) {
			return any_cast<bool>(it->second);
		} else {
			LOG_FATAL("The key doesn't match a string value");
		}
	}

	return defaultValue;
}

int Settings::get(const std::string & key, int defaultValue) const {
	Keys::const_iterator it = _keyMap.find(key);
	if (it != _keyMap.end()) {
		if (isInteger(it->second)) {
			return any_cast<int>(it->second);
		} else {
			LOG_FATAL("The key doesn't match a string value");
		}
	}

	return defaultValue;
}

std::string Settings::serialize() {
	return "";
}

bool Settings::unserialize(const std::string & /*data*/) {
	return true;
}

bool Settings::isBoolean(const boost::any & operand) const {
	return (operand.type() == typeid(bool));
}

bool Settings::isInteger(const boost::any & operand) const {
	return (operand.type() == typeid(int));
}

bool Settings::isString(const boost::any & operand) const {
	return any_cast<std::string>(&operand);
}
