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

#ifndef DATABASE_XMLCONFIGREADER_H
#define DATABASE_XMLCONFIGREADER_H

#include "config/Config.h"

class QDomElement;

namespace database {

/**
 * Reads the configuration/settings of the application from a XML file
 * and creates a Config object.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class XmlConfigReader {
public:

	XmlConfigReader(const QString & data);

	~XmlConfigReader() {
	}

	Config & getConfig() const {
		return _config;
	}

private:

	XmlConfigReader();
	XmlConfigReader(const XmlConfigReader &);
	XmlConfigReader & operator=(const XmlConfigReader &);

	/// Init advanced settings
	void XmlAdvConfReader(QDomElement element);

	/// Init video settings
	void XmlVideoConfReader(QDomElement element);

	/// Load audio settings
	void XmlAudioReader(QDomElement element);

	/// load input audio settings
	void XmlInputReader(QDomElement element);

	/// load output audio settings
	void XmlOutputReader(QDomElement element);

	/// load ringing audio settings
	void XmlRingingReader(QDomElement element);

	Config & _config;
};

}

#endif	//DATABASE_XMLCONFIGREADER_H
