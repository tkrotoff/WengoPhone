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

#ifndef OWPATH_H
#define OWPATH_H

#include <cutil/global.h>

#include <util/owutildll.h>
#include <util/NonCopyable.h>

#include <string>

/**
 * Path class.
 *
 * Give following services:
 * - application path
 * - resources path
 * - config path
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Path : NonCopyable {
public:

	/**
	 * Gets the directory that contains the application executable.
	 *
	 * The executable path returned always finished by "/" or "\".
	 *
	 * On MacOS X, returns full path to exe. e.g: /Application/WengoPhone.app/Contents/MacOS/WengoPhone
	 *
	 * @return application executable path
	 */
	OWUTIL_API static std::string getApplicationDirPath();

	/**
	 * Gets the configuration directory.
	 *
	 * @return the configuration directory path
	 */
	OWUTIL_API static std::string getConfigurationDirPath();

	/**
	 * @see File::getPathSeparator()
	 */
	OWUTIL_API static std::string getPathSeparator();

	/**
	 * Gets the home directory path.
	 *
	 * @return the home dir of the current user.
	 */
	OWUTIL_API static std::string getHomeDirPath();

#ifdef OS_MACOSX
	//Special extensions for MacOS X

	/**
	 * Gets the application bundle path.
	 *
	 * e.g: /Application/WengoPhone.app/
	 */
	OWUTIL_API static std::string getApplicationBundlePath();

	/**
	 * Gets the Private Frameworks path.
	 *
	 * e.g: full-path-tp-application.app/Contents/Frameworks
	 */
	OWUTIL_API static std::string getApplicationPrivateFrameworksDirPath();

	/**
	 * Gets the Resources path.
	 *
	 * e.g: full-path-tp-application.app/Contents/Resources
	 */
	OWUTIL_API static std::string getApplicationResourcesDirPath();
#endif
};

#endif	//OWPATH_H
