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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <StringList.h>
#include <Mutex.h>

#include <map>

class Config;

/**
 * Manages configuration profiles.
 *
 * This class contains every available Config.
 * There is at least one Config named "default"
 *
 * @see Config
 * @ingroup model
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class ConfigManager {
public:

	static ConfigManager & getInstance();

	/**
	 * Gets the current config.
	 *
	 * @return a reference to the current config
	 */
	Config & getCurrentConfig() const;

	/**
	 * Sets the current Config.
	 *
	 * If the Config does not exist, the previous one is kept.
	 *
	 * @param config the name of the config to set as current
	 */
	void setCurrentConfig(const std::string & configName);

	/**
	 * Gets the Config list.
	 *
	 * @return the list of all Config name
	 */
	StringList getConfigList() const;

	/**
	 * Gets a Config by its name.
	 *
	 * @param name name of the desired Config
	 * @return the desired Config, NULL if it does not exist.
	 */
	Config * getConfig(const std::string & configName) const;

	/**
	 * Adds a new Config.
	 *
	 * @param config the Config to add
	 */
	void addConfig(Config * config);

private:

	ConfigManager();

	~ConfigManager();

	typedef std::map<const std::string, Config *> ConfigList;

	ConfigList _configList;

	Config * _currentConfig;

	static Mutex _mutex;
};

#endif	//CONFIGMANAGER_H
