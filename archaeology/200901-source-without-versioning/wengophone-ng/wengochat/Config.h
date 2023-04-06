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

#ifndef OWCONFIG_H
#define OWCONFIG_H

#include <coipmanager/CoIpManagerConfig.h>

#include <util/Singleton.h>

/**
 * Stores the configuration options of WengoPhone.
 *
 * Tries to make it impossible to make a mistake.
 *
 * @see CoIpManagerConfig
 * @see AutomaticSettings
 * @see Settings
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Config : public Singleton<Config>, public CoIpManagerConfig {
	friend class Singleton<Config>;
public:

	/**
	 * Current/last configuration version.
	 *
	 * Used to import config from previous version of Wengophone
	 */
	static const int CONFIG_VERSION;

	/** Gets config version number. */
	static const std::string CONFIG_VERSION_KEY;
	int getConfigVersion() const;

	/** Gets the id of the last used UserProfile. */
	static const std::string PROFILE_LAST_USED_ID_KEY;
	std::string getProfileLastUsedId() const;

protected:

	Config();

	virtual ~Config();
};

#endif	//OWCONFIG_H
