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

#ifndef XMLCONFIGWRITER_H
#define XMLCONFIGWRITER_H

#include <NonCopyable.h>

#include <string>

class Contact;
class QDomDocument;
class QDomElement;

/**
 * Writes the optionnal settings of a Contact to a XML file.
 *
 * @author Tanguy Krotoff
 */
class XmlContactWriter : NonCopyable {
public:

	static const std::string WENGO_TAG;

	static const std::string CONTACT_TAG;

	static const std::string BLOCKED_TAG;

	XmlContactWriter(const Contact & contact);

	const std::string & toString() const {
		return _data;
	}

private:

	static QDomElement createBooleanNode(QDomDocument & doc, const std::string & tag, bool value);

	static QDomElement createTextNode(QDomDocument & doc, const std::string & tag, const std::string & value);

	std::string _data;
};

#endif	//XMLCONFIGWRITER_H
