/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <Startup.h>

#include <StringList.h>

#ifdef WIN32
#include <windows.h>
#include <winreg.h>
#endif	//WIN32

#include <iostream>
using namespace std;

static const char * STARTUP_REGISTRY_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\";

Startup::Startup(const std::string & applicationName, const std::string & executablePath) {
	_applicationName = applicationName;
	_executablePath = executablePath;
}

Startup::~Startup() {
}

bool Startup::setStartup(bool startup) {
#ifdef WIN32
	HKEY hKey;
	::RegOpenKeyExA(HKEY_CURRENT_USER, STARTUP_REGISTRY_KEY,
				0, KEY_WRITE, &hKey);

	if (startup) {
		::RegCreateKeyA(HKEY_CURRENT_USER, STARTUP_REGISTRY_KEY, &hKey);

		if (ERROR_SUCCESS == ::RegSetValueExA(hKey, _applicationName.c_str(), 0, REG_SZ,
				(const BYTE *) _executablePath.c_str(), _executablePath.length())) {
			::RegCloseKey(hKey);
			return true;
		}
	} else {
		string empty = "";
		if (ERROR_SUCCESS == ::RegSetValueExA(hKey, _applicationName.c_str(), 0, REG_SZ,
				(const BYTE *) empty.c_str(), empty.length())) {
			::RegCloseKey(hKey);
			return true;
		}
	}

	::RegCloseKey(hKey);
	return false;
#else
	startup = false;
	return startup;
#endif	//WIN32
}

bool Startup::isStartup() {
#ifdef WIN32
	HKEY hKey;

	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_CURRENT_USER, STARTUP_REGISTRY_KEY,
					0, KEY_QUERY_VALUE, &hKey)) {

		DWORD dwDataType = REG_SZ;
		DWORD dwSize = 512;
		char executablePathKeyValue[512];

		if (ERROR_SUCCESS == ::RegQueryValueExA(hKey, _applicationName.c_str(), 0, &dwDataType,
					(BYTE *) executablePathKeyValue, &dwSize)) {

			::RegCloseKey(hKey);

			if (!dwSize)
				return false;
			
			String regKey(executablePathKeyValue);
			String exePath(_executablePath);
			String appName(_applicationName);
			regKey = regKey.toLowerCase();
			exePath = exePath.toLowerCase();
			appName = appName.toLowerCase();

			if (regKey.find(exePath) != string::npos ||
				regKey.find(appName) != string::npos) {

				return true;
			}
		}
	}

	::RegCloseKey(hKey);
	return false;
#else
	return false;
#endif	//WIN32
}
