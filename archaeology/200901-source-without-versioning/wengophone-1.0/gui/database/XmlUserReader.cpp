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

#include "XmlUserReader.h"
using namespace database;

#include <qdom.h>
#include <qstring.h>

XmlUserReader::XmlUserReader(const QString & data) {
	QString login;
	QString password;
	bool autoLogin = false;

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
		if (element.tagName() == "login") {
			login = element.text();
		}
		if (element.tagName() == "password") {
			password = element.text();
		}
		if (element.tagName() == "autoLogin") {
			autoLogin = (element.text() == "true");
		}
		node = node.nextSibling();
	}
	_user = User(login, password, autoLogin);
}
