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

#include "XmlContactReader.h"

#include "XmlContactWriter.h"
#include "Contact.h"

#include <qdom.h>
#include <qstring.h>

using namespace std;

XmlContactReader::XmlContactReader(const std::string & data, Contact & contact) {
	QDomDocument doc;
	if (!doc.setContent(data)) {
		return;
	}

	//Print out the element names of all elements that are direct
	//children of the outermost element
	QDomElement docElem = doc.documentElement();

	QDomNode node = docElem.firstChild();
	while (!node.isNull()) {
		//Try to convert the node to an element
		QDomElement element = node.toElement();

		if (!element.isNull()) {
			//The node really is an element

			string tag = element.tagName();

			//WengoPhone
			if (tag == XmlContactWriter::WENGO_TAG) {
				contact.setWengoPhone(element.text());
			}

			//Blocked
			if (tag == XmlContactWriter::BLOCKED_TAG) {
				contact.setBlocked(readBooleanNode(element));
			}

		}
		node = node.nextSibling();
	}
}

bool XmlContactReader::readBooleanNode(const QDomElement & element) {
	string text = element.text();
	if (text == "true") {
		return true;
	} else {
		return false;
	}
}
