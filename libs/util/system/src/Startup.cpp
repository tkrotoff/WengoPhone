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

#include <system/Startup.h>

#include <util/String.h>

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <system/WindowsRegistry.h>
#endif

#include <iostream>
using namespace std;

#ifdef OS_WINDOWS
static const char * STARTUP_REGISTRY_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";
#endif

Startup::Startup(const std::string & applicationName, const std::string & executablePath)
	: _applicationName(applicationName),
	_executablePath(executablePath) {
}

Startup::~Startup() {
}

bool Startup::setStartup(bool startup) {
#ifdef OS_WINDOWS

	//add the entry to the key
	if (startup) {
		return WindowsRegistry::createEntry(HKEY_CURRENT_USER, std::string(STARTUP_REGISTRY_KEY), _applicationName, _executablePath);
	//remove the entry from the key
	} else {
		return WindowsRegistry::removeEntry(HKEY_CURRENT_USER, std::string(STARTUP_REGISTRY_KEY), _applicationName);
	}
#else
	startup = false;
	return startup;
#endif	//OS_WINDOWS
}

bool Startup::isStartup() {
#ifdef OS_WINDOWS
	return WindowsRegistry::entryExists(HKEY_CURRENT_USER, std::string(STARTUP_REGISTRY_KEY), _applicationName);
#else
	return false;
#endif	//OS_WINDOWS
}
