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

#ifndef CLASSICCONFIGIMPORTER_H
#define CLASSICCONFIGIMPORTER_H

#include <string>

/**
 * Import configuration from WengoPhone classic.
 *
 * @ingroup model
 * @author Philippe Bernery
 */
class ClassicConfigImporter {
public:

	/**
	 * Imports the WengoPhone Classic Config only if no WengoPhone NG
	 * configuration exists in the given folder.
	 *
	 * @param path folder where the WengoPhone NG configuration files could be
	 * @return true if configuration has been imported, false if no configuration
	 * found or the configuration has already been imported.
	 */
	static bool importConfig(const std::string & import);

};

#endif	//CLASSICCONFIGIMPORTER_H
