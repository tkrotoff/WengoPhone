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

#ifndef DATABASE_XMLAUTHWRITER_H
#define DATABASE_XMLAUTHWRITER_H

#include <qstring.h>

class QDomDocument;
class QDomElement;
class Authentication;

namespace database {

/**
 * Writes the configuration/settings of the application from
 * a Config object to a XML file.
 *
 * @author Tanguy Krotoff
 */
class XmlAuthWriter {
public:

	XmlAuthWriter(const Authentication & auth);

	QString getData() const {
		return _data;
	}

private:

	/**
	 * Translates authentication informations into XML data.
	 *
	 * @param doc xml document
	 * @param auth authentication data
	 * @return XML data for Authentication
	 */
	QDomElement writer(QDomDocument & doc, const Authentication & auth);

	/**
	 * Creates a XML element and insert it in the DOM tree.
	 *
	 * @param doc xml document
	 * @param tag name tag
	 * @param data text of the XML element
	 * @return a XML element
	 */
	static QDomElement createElement(QDomDocument & doc, const QString & tag, const QString & data);

	/**
	 * Create a XML element and insert it in the DOM tree.
	 *
	 * @param doc xml document
	 * @param tag name tag
	 * @param data text of the XML element
	 * @return a XML element
	 */
	static QDomElement createElement(QDomDocument & doc, const QString & tag, const int data);

	XmlAuthWriter();
	XmlAuthWriter(const XmlAuthWriter &);
	XmlAuthWriter & operator=(const XmlAuthWriter &);

	QString _data;
};

}

#endif	//DATABASE_XMLAUTHWRITER_H
