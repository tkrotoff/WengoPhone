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

#ifndef DATABASE_XMLCONFIGWRITER_H
#define DATABASE_XMLCONFIGWRITER_H

#include "config/Config.h"
#include "config/Audio.h"
#include "config/AdvancedConfig.h"
#include "config/Video.h"

class QString;
class QDomDocument;
class QDomElement;

namespace database {

/**
 * Writes the configuration/settings of the application from
 * a Config object to a XML file.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class XmlConfigWriter {
public:

	XmlConfigWriter(const Config & config);

	QString getData() const {
		return _data;
	}

	/// create XML data for audio settings
	QDomElement XmlAudioWriter(QDomDocument & doc, const Audio & audio);

	// create XML data for advanced config
	QDomElement XmlAdvConfWriter(QDomDocument & doc, AdvancedConfig & advConf);

	// create XML data for advanced config
	QDomElement XmlVideoConfWriter(QDomDocument & doc, const Video & videoConf);

private:

	XmlConfigWriter();
	XmlConfigWriter(const XmlConfigWriter &);
	XmlConfigWriter & operator=(const XmlConfigWriter &);

	/// create a XML element and insert it in the DOM tree
	QDomElement createElement(QDomDocument & doc, const QString & tag, const QString & data);

	/// create a XML element and insert it in the DOM tree
	QDomElement createElement(QDomDocument & doc, const QString & tag, const int data);

	QString _data;
};

}

#endif	//DATABASE_XMLCONFIGWRITER_H
