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
 
#include "ClassicConfigImporter.h"

#include "ConfigManager.h"
#include "Config.h"

#include <cutil/global.h>
#include <util/Path.h>
#include <util/File.h>

using namespace std;
 
bool ClassicConfigImporter::importConfig(const std::string & import) {

#if (defined(OS_WINDOWS) || defined(OS_LINUX))

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (!File::exists(config.getConfigDir()) && File::exists(getWengoClassicConfigPath())) {
		return true;
	}

#endif

	return false;
}

string ClassicConfigImporter::getWengoClassicConfigPath() {
	string result;

#if defined(OS_WINDOWS)

	result = File::convertPathSeparators(Path::getHomeDirPath() + "wengo/");

#elif defined(OS_LINUX)

	result = File::convertPathSeparators(Path::getHomeDirPath() + ".wengo/");

#endif

	return result;
}
