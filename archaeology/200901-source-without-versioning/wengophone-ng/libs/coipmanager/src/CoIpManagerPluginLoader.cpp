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

#include <coipmanager/CoIpManagerPluginLoader.h>

#include <shlibloader/SharedLibLoader.h>

#include <util/String.h>
#include <util/Logger.h>
#include <util/StringList.h>
#include <util/File.h>
#include <util/Path.h>

#include <cutil/global.h>

std::list<void *> CoIpManagerPluginLoader::loadPlugins(const StringList & pluginsPaths,
	const std::string & pluginsSubDir, const std::string & pluginsName) {

	std::list<void *> listPlugin;

	for (StringList::const_iterator it1 = pluginsPaths.begin(); it1 != pluginsPaths.end(); ++it1) {
		std::string pluginDir(*it1 + pluginsSubDir);

		File dir(pluginDir);
		StringList fileList = dir.getFileList();
		if (fileList.empty()) {
			LOG_DEBUG(pluginDir + ": empty directory");
			continue;
		}

		for (StringList::const_iterator it = fileList.begin(); it != fileList.end(); ++it) {
			String pluginName = *it;

#ifdef OS_WINDOWS
			if (!pluginName.contains(".dll", false)) {
				continue;
			}
#else
			if (!pluginName.contains(".so", false)) {
				continue;
			}
#endif
			std::string pluginFullPath = dir.getPath() + pluginName;

			void * coip_plugin_init = SharedLibLoader::resolve(pluginFullPath, "coip_" + pluginsName + "_plugin_init");

			if (coip_plugin_init) {
				LOG_DEBUG(pluginFullPath + " plugin loaded");
				listPlugin.push_back(coip_plugin_init);
			} else {
				LOG_ERROR(pluginFullPath + " plugin not loaded");
			}
		}

		//If listPlugin is not empty
		//this means that pluginDir is actually the right plugin directory
		//then no need to look at the other directories
		if (!listPlugin.empty()) {
			break;
		}

	}

	if (listPlugin.empty()) {
		LOG_DEBUG("couldn't find the plugins: " + pluginsPaths.toString());
	}

	return listPlugin;
}
