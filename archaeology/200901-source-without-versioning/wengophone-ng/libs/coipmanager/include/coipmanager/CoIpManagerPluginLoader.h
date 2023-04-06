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

#ifndef OWCOIPMANAGERPLUGINLOADER_H
#define OWCOIPMANAGERPLUGINLOADER_H

#include <util/NonCopyable.h>
#include <util/StringList.h>

/**
 * CoIpManager plugins loader.
 *
 * A plugin is a shared library (.dll, .so) loaded at runtime.
 *
 * Code factorization: internal to CoIpManager, no dllexport
 *
 * @author Tanguy Krotoff
 */
class CoIpManagerPluginLoader : NonCopyable {
public:

	/**
	 * Loads a CoIpManager plugins.
	 *
	 * Uses SharedLibLoader and returns the function pointer for each
	 * plugin found.
	 *
	 * @param pluginsPaths list of path where the plugins are located
	 * @param pluginsSubDir concatenated directory to pluginsPaths
	 * @param pluginsName used to get the right function name to call for loading plugins.
	 *   The coip_'pluginsName'_plugin_init function will be called.
	 * @return list of functions pointer (1 function pointer = 1 plugin)
	 */
	static std::list<void *> loadPlugins(const StringList & pluginsPaths,
		const std::string & pluginsSubDir, const std::string & pluginsName);
};

#endif	//OWCOIPMANAGERPLUGINLOADER_H
