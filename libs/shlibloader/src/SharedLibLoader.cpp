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

#include <shlibloader/SharedLibLoader.h>

#include <global.h>
#include <StringList.h>

#if defined(OS_WINDOWS)
	#include "win32/Win32SharedLibLoader.h"
#elif defined(OS_POSIX)
	#include "posix/PosixSharedLibLoader.h"
#else
	#include "null/NullSharedLibLoader.h"
#endif

SharedLibLoader::SharedLibLoader(const std::string & fileName) {
	_loaderPrivate = NULL;
	String tmp(fileName);

#if defined(OS_WINDOWS)
	if (!tmp.contains(".dll", false)) {
		tmp.append(".dll");
	}
#elif defined(OS_LINUX)
	if (!tmp.contains(".so", false)) {
		tmp.append(".so");
	}
#elif defined(OS_MACOSX)
	if (!tmp.contains(".dylib", false)) {
		tmp.append(".dylib");
	}
#endif

#if defined(OS_WINDOWS)
	_loaderPrivate = new Win32SharedLibLoader(fileName);
#elif defined(OS_POSIX)
	_loaderPrivate = new PosixSharedLibLoader(fileName);
#else
	_loaderPrivate = new NullSharedLibLoader(fileName);
#endif
}

SharedLibLoader::~SharedLibLoader() {
	delete _loaderPrivate;
}

bool SharedLibLoader::load() {
	return _loaderPrivate->load();
}

bool SharedLibLoader::unload() {
	return _loaderPrivate->unload();
}

void * SharedLibLoader::resolve(const std::string & symbol) {
	return _loaderPrivate->resolve(symbol);
}

void * SharedLibLoader::resolve(const std::string & fileName, const std::string & symbol) {
	SharedLibLoader loader(fileName);
	loader.load();
	return loader.resolve(symbol);
}
