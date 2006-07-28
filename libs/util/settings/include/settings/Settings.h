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

#ifndef OWSETTINGS_H
#define OWSETTINGS_H

#include <serialization/Serializable.h>

#include <util/StringList.h>
#include <util/Event.h>
#include <thread/Mutex.h>

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
 * @author Mathieu Stute
 */
class Settings {
	friend class SettingsXMLSerializer;
public:

	/**
	 * A value has changed inside Settings.
	 *
	 * @param sender this class
	 * @param key key whose value has changed
	 */
	Event<void (Settings & sender, const std::string & key)> valueChangedEvent;

	Settings();

	Settings(const Settings & settings);

	Settings & operator=(const Settings & settings);

	virtual ~Settings();

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
	 * Checks if the specified key exists.
	 *
	 * @return true if there exists a setting called key; returns false otherwise
	 */
	bool contains(const std::string & key) const;

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
	void set(const std::string & key, const StringList & value);

	/**
	 * @see set()
	 */
	void set(const std::string & key, bool value);

	/**
	 * @see set()
	 */
	void set(const std::string & key, int value);

	/**
	 * @see set()
	 */
	void set(const std::string & key, boost::any value);

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
	StringList get(const std::string & key, const StringList & defaultValue) const;

	/**
	 * @see get()
	 */
	bool get(const std::string & key, bool defaultValue) const;

	/**
	 * @see get()
	 */
	int get(const std::string & key, int defaultValue) const;

	/**
	 * Gets the value for a given key in a generic manner.
	 *
	 * @see get()
	 */
	boost::any getAny(const std::string & key, const boost::any & defaultValue) const;

	/**
	 * @return number of elements
	 */
	unsigned size() const {
		return _keyMap.size();
	}

	/**
	 * Checks if the value is a boolean.
	 *
	 * @param value to check
	 * @return true if value is a boolean; false otherwise
	 */
	static bool isBoolean(const boost::any & value);

	/**
	 * Checks if the value is an integer.
	 *
	 * @param value to check
	 * @return true if value is an integer; false otherwise
	 */
	static bool isInteger(const boost::any & value);

	/**
	 * Checks if the value is a std::string.
	 *
	 * @param value to check
	 * @return true if value is a std::string; false otherwise
	 */
	static bool isString(const boost::any & value);

	/**
	 * Checks if the value is a StringList.
	 *
	 * @param value to check
	 * @return true if value is a StringList; false otherwise
	 */
	static bool isStringList(const boost::any & value);

protected:

	virtual void copy(const Settings & settings);

	typedef std::map<const std::string, boost::any> Keys;
	Keys _keyMap;

private:

	/** Mutex for thread-safe. */
	mutable Mutex _mutex;
};

#endif	//OWSETTINGS_H
