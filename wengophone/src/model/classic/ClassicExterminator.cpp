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


#include "ClassicExterminator.h"

#include <cutil/global.h>

#include <system/Processes.h>
#include <system/Startup.h>

#include <stdio.h>

#ifdef OS_WINDOWS
#include <windows.h>
static const char * STARTUP_REGISTRY_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const char * WENGO_VALUE = "Wengo";
#endif

static const char * CLASSIC_EXECUTABLE_NAME = "wengophone.exe";

void ClassicExterminator::removeClassicFromStartup() {
#ifdef OS_WINDOWS
	Startup startup(WENGO_VALUE, "");
	startup.setStartup(false);
#endif
}

void ClassicExterminator::killClassicExecutable() {
	Processes::killProcess(std::string(CLASSIC_EXECUTABLE_NAME));
}
