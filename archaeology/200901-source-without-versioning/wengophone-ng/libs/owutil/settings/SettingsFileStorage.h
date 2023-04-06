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

#ifndef OWSETTTINGSFILESTORAGE_H
#define OWSETTTINGSFILESTORAGE_H

#include <settings/Settings.h>

#include <util/NonCopyable.h>

/**
 * Stores class Settings into a file.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class SettingsFileStorage : NonCopyable {
public:

	/**
	 * Loads settings from a file.
	 *
	 * @param filename file where to load the settings
	 * @param settings loaded from the file
	 * @return true if settings were loaded; false otherwise
	 */
	OWSETTINGS_API static bool load(const std::string & filename, Settings & settings);

	/**
	 * Saves the settings to a file.
	 *
	 * @param filename file where to save the settings
	 * @param settings to save
	 * @return true if settings were saved; false otherwise
	 */
	OWSETTINGS_API static bool save(const std::string & filename, Settings settings);
};

#endif	//OWSETTTINGSFILESTORAGE_H
