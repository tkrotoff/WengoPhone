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

#include "XmlAuthReader.h"
using namespace database;

#include "config/Authentication.h"

#include <qdom.h>
#include <qstring.h>

#include <iostream>
using namespace std;

XmlAuthReader::XmlAuthReader(const QString & data) {
	QDomDocument doc;
	if (!doc.setContent(data)) {
		return;
	}

	//Print out the element names of all elements that are direct
	//children of the outermost element
	QDomElement docElem = doc.documentElement();

	read(docElem);
}

void XmlAuthReader::read(const QDomElement & element) {
	Authentication & auth = Authentication::getInstance();
	QDomElement elem;

	QDomNode node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();

		if (elem.tagName() == "displayname") {
			auth.setDisplayName(elem.text());
		}

		if (elem.tagName() == Authentication::USERNAME_TAG) {
			auth.setUsername(elem.text());
		}

		if (elem.tagName() == Authentication::USERID_TAG) {
			auth.setUserId(elem.text());
		}

		if (elem.tagName() == Authentication::PASSWORD_TAG) {
			auth.setPassword(elem.text());
		}

		if (elem.tagName() == Authentication::REALM_TAG) {
			auth.setRealm(elem.text());
		}
		
		if (elem.tagName() == Authentication::SERVER_TAG) {
			XmlAuthServerReader(elem.toElement());
		}
		
		if (elem.tagName() == Authentication::PROXY_TAG) {
			XmlAuthProxyReader(elem.toElement());
		}
		
		if (elem.tagName() == Authentication::HTTP_TUNNEL_TAG) {
			QString host = getElementText(elem, "host");
			if (!host.isNull()) {
				auth.setHttpTunnelHost(host);
			}
		}

		if (elem.tagName() == Authentication::AUTO_REGISTER_TAG) {
			auth.setAutoRegister(elem.text() == "true");
		}

		if (elem.tagName() == "status") {
			auth.setStatusCode(elem.attribute("code", "401").toInt(), elem.text());
		}

		if (elem.tagName() == Authentication::FORCE_REGISTER_TAG) {
			auth.setForceRegister(elem.text() == "1");
		}
		
		node = node.nextSibling();
	}
}

void XmlAuthReader::XmlAuthProxyReader(QDomElement element) {
	Authentication & auth = Authentication::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Authentication::PROXY_HOST_TAG) {
			auth.setProxy(elem.text());
		}
		if (elem.tagName() == Authentication::PORT_TAG) {
#ifdef DEBUG
			cout << "\nset proxy port :" << elem.text() << endl;
#endif
			auth.setPort(elem.text());
		}
		node = node.nextSibling();
	}
}

void XmlAuthReader::XmlAuthServerReader(QDomElement element) {
	Authentication & auth = Authentication::getInstance();
	QDomNode node;
	QDomElement elem;

	node = element.firstChild();
	while (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == Authentication::SERVER_HOST_TAG) {
			auth.setServer(elem.text());
		}
		if (elem.tagName() == Authentication::SERVER_PORT_TAG) {
			auth.setServerPort(elem.text());
		}
		node = node.nextSibling();
	}
}


QString XmlAuthReader::getElementText(const QDomElement & element, const QString & tag) {
	QString text;
	QDomElement elem;

	QDomNode node = element.firstChild();
	if (!node.isNull()) {
		elem = node.toElement();
		if (elem.tagName() == tag) {
			text = elem.text();
		}
	}
	//node = node.nextSibling();
	return text;
}

Authentication & XmlAuthReader::getAuth() const {
	return Authentication::getInstance();
}

