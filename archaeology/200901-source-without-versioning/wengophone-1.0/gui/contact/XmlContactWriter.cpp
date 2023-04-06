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

#include "XmlContactWriter.h"

#include "Contact.h"

#include <qdom.h>
#include <qstring.h>

using namespace std;

const std::string XmlContactWriter::WENGO_TAG = "wengo";
const std::string XmlContactWriter::CONTACT_TAG = "contact";
const std::string XmlContactWriter::BLOCKED_TAG = "blocked";

XmlContactWriter::XmlContactWriter(const Contact & contact) {
	QDomDocument doc;
	QDomElement root = doc.createElement(CONTACT_TAG);
	doc.appendChild(root);

	//WengoPhone
	root.appendChild(createTextNode(doc, WENGO_TAG, contact.getWengoPhone()));

	//Blocked
	root.appendChild(createBooleanNode(doc, BLOCKED_TAG, contact.isBlocked()));


	_data = doc.toString().ascii();
}

QDomElement XmlContactWriter::createBooleanNode(QDomDocument & doc, const std::string & tag, bool value) {
	if (value) {
		return createTextNode(doc, tag, "true");
	} else {
		return createTextNode(doc, tag, "false");
	}
}

QDomElement XmlContactWriter::createTextNode(QDomDocument & doc, const std::string & tag, const std::string & value) {
	QDomElement element = doc.createElement(tag);
	QDomText text = doc.createTextNode(value);
	element.appendChild(text);
	return element;
}
