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

#include "XmlAuthWriter.h"
using namespace database;

#include "config/Authentication.h"

#include <qdom.h>
#include <qstring.h>

XmlAuthWriter::XmlAuthWriter(const Authentication & auth) {
	QDomDocument doc;

	doc.appendChild(writer(doc, auth));

	_data = doc.toString();
}

QDomElement XmlAuthWriter::writer(QDomDocument & doc, const Authentication & auth) {
	//Records elements
	QString username = auth.getUsername();
	QString userId = auth.getUserId();
	QString password = auth.getPassword();
	QString realm = auth.getRealm();
	QString server = auth.getServer();
	QString proxy = auth.getProxy();
	QString port = auth.getPort();
	QString httpTunnelHost = auth.getHttpTunnelHost();
	QDomElement root = doc.createElement(Authentication::AUTH_TAG);

	//Auto register
	if (auth.isAutoRegister())
		root.appendChild(
				createElement(doc,
				Authentication::AUTO_REGISTER_TAG,
				"true"));
	else
		root.appendChild(
				createElement(doc,
				Authentication::AUTO_REGISTER_TAG,
				"false"));

	// force register
	if (auth.isForceRegister())
		root.appendChild(
				createElement(doc,
				Authentication::FORCE_REGISTER_TAG,
				"1"));
	else
		root.appendChild(
				createElement(doc,
				Authentication::FORCE_REGISTER_TAG,
				"0"));

	//Username
	if (!username.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::USERNAME_TAG,
				username));

	//User id
	if (!userId.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::USERID_TAG,
				userId));

	//Password
	if (!password.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::PASSWORD_TAG,
				password));

	//Realm
	if (!realm.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::REALM_TAG,
				realm));

	//Server
	if (!server.isEmpty()) {
		QDomElement serverTag = doc.createElement("server");
		root.appendChild(serverTag);
		serverTag.appendChild(
				createElement(doc,
				"host",
				server));
	}

	//Proxy
	if (!proxy.isEmpty()) {
		QDomElement proxyTag = doc.createElement("proxy");
		root.appendChild(proxyTag);
		proxyTag.appendChild(
				createElement(doc,
				"host",
				proxy));
	}

	//Port
	if (!port.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::PORT_TAG,
				port));

	//Http tunnel host
	if (!httpTunnelHost.isEmpty())
		root.appendChild(
				createElement(doc,
				Authentication::HTTP_TUNNEL_TAG,
				httpTunnelHost));

	return root;
}

QDomElement XmlAuthWriter::createElement(QDomDocument & doc, const QString & tag, const QString & data) {
	QDomElement element = doc.createElement(tag);
	QDomText text = doc.createTextNode(data);
	element.appendChild(text);
	return element;
}

QDomElement XmlAuthWriter::createElement(QDomDocument & doc, const QString & tag, const int data) {
	QDomElement element = doc.createElement(tag);
	QDomText text = doc.createTextNode(QString("%1").arg(data));
	element.appendChild(text);
	return element;
}
