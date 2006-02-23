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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Serializable.h>

#include <StringList.h>
#include <Event.h>

#include <boost/any.hpp>
#include <map>

/**
 * Persistent platform-independent application settings.
 *
 * @see QSettings
 * @see java.util.Properties
 * @see java.util.Preferences
 * @see commoncpp.Keydata
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class Settings : public Serializable {
public:

	/**
	 * A value has changed inside Settings.
	 *
	 * @param sender this class
	 * @param key key whose value has changed
	 */
	Event<void (Settings & sender, const std::string & key)> valueChangedEvent;

	Settings();

	/**
	 * Gets all the keys.
	 *
	 * @return list of all keys, including subkeys
	 */
	StringList getAllKeys() const;

	/**
	 * Removes the setting key.
	 * Can remove a group of keys. e.g: settings contains:
	 * - "audio.data"
	 * - "audio.speakers.left"
	 * Using remove("audio") will remove "audio.data" and "audio.speakers.left"
	 *
	 * @param key key to remove
	 */
	void remove(const std::string & key);

	/**
	 * Check if the specified key exists.
	 *
	 * @return true if there exists a setting called key; returns false otherwise
	 */
	bool contains(const std::string & key);

	/**
	 * Sets the value of a key.
	 *
	 * @param key the key
	 * @param value key value
	 */
	void set(const std::string & key, const std::string & value);

	/**
	 * @see set()
	 */
	void set(const std::string & key, bool value);

	/**
	 * @see set()
	 */
	void set(const std::string & key, int value);

	/**
	 * Gets the value for a given key.
	 *
	 * @param key the key
	 * @param defaultValue default value to return if no other value
	 * @return the value associated with the key or the default value if the value doesn't exist
	 */
	std::string get(const std::string & key, const std::string & defaultValue) const;

	/**
	 * @see get()
	 */
	bool get(const std::string & key, bool defaultValue) const;

	/**
	 * @see get()
	 */
	int get(const std::string & key, int defaultValue) const;

	/**
	 * @return number of elements
	 */
	unsigned size() const {
		return _keyMap.size();
	}

	std::string serialize();

	bool unserialize(const std::string & data);

private:

	bool isBoolean(const boost::any & operand) const;

	bool isInteger(const boost::any & operand) const;

	bool isString(const boost::any & operand) const;

	typedef std::map<const std::string, boost::any> Keys;

	Keys _keyMap;
};

#endif	//SETTINGS_H
