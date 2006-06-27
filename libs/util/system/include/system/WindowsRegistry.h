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

#ifndef OW_WINDOWSREGISTRY_H
#define OW_WINDOWSREGISTRY_H

#include <windows.h>

#include <string>

/**
 * 
 *
 * @author Mathieu Stute
 */
class WindowsRegistry {
public:

	/**
	 * test the existence of a registry key.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param key the key to test.
	 * @return true if exists otherwise return false.
	 */
	static bool keyExists(HKEY rootKey, const std::string & key);

	/**
	 * test the existence of a registry entry.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the search the entry.
	 * @param entry the entry to test.
	 * @return true if exists otherwise return false.
	 */
	static bool entryExists(HKEY rootKey, const std::string & subkey, const std::string & entry);

	/**
	 * return the value of a registry entry.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the search the entry.
	 * @param entry the entry to test.
	 * @return the value of the entry if success otherwise an empty string.
	 */
	static std::string getValue(HKEY rootKey, const std::string & subkey, const std::string & entry);

	/**
	 * create a registry entry.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the key must be created.
	 * @param entry the name of the key.
	 * @return true if success otherwise false.
	 */
	static bool createKey(HKEY rootKey, const std::string & subkey, const std::string & key, const std::string & defaultValue = "");

	/**
	 * delete a registry entry.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the key is.
	 * @param entry the name of the key.
	 * @return true if success otherwise false.
	 */
	static bool deleteKey(HKEY rootKey, const std::string & subkey, const std::string & key);

	/**
	 * create an entry in a registry key.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the entry will be.
	 * @param entry the name of the entry.
	 * @param value value of the entry.
	 * @return true if success otherwise false.
	 */
	static bool createEntry(HKEY rootKey, const std::string & subkey, const std::string & entry, const std::string & value);

	/**
	 * remove an entry to a registry key.
	 *
	 * @param rootKey the root key of type HKEY (HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, ...).
	 * @param subkey the subkey where the entry is.
	 * @param entry the name of the entry.
	 * @return true if success otherwise false.
	 */
	static bool removeEntry(HKEY rootKey, const std::string & subkey, const std::string & entry);
};
#endif	//OW_WINDOWSREGISTRY_H