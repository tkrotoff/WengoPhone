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

#include "XmlUserWriter.h"
using namespace database;

#include "login/User.h"

#include <qdom.h>
#include <qstring.h>

XmlUserWriter::XmlUserWriter(const User & user) {
	QDomDocument doc;
	doc.appendChild(createElement(doc, user));

	_data = doc.toString();
}

QDomElement XmlUserWriter::createElement(QDomDocument & doc, const User & user) {
	QDomElement userTag = doc.createElement("user");

	QDomElement loginTag = doc.createElement("login");
	userTag.appendChild(loginTag);
	QDomCDATASection loginText = doc.createCDATASection(user.getLogin().utf8());
	loginTag.appendChild(loginText);

	QDomElement passwordTag = doc.createElement("password");
	userTag.appendChild(passwordTag);
	QDomCDATASection passwordText = doc.createCDATASection(user.getPassword().utf8());
	passwordTag.appendChild(passwordText);

	/*QDomElement loginTag = doc.createElement("login");
	userTag.appendChild(loginTag);
	QDomText loginText = doc.createTextNode(user.getLogin());
	loginTag.appendChild(loginText);

	QDomElement passwordTag = doc.createElement("password");
	userTag.appendChild(passwordTag);
	QDomText passwordText = doc.createTextNode(user.getPassword());
	passwordTag.appendChild(passwordText);*/

	QDomElement autoLoginTag = doc.createElement("autoLogin");
	userTag.appendChild(autoLoginTag);
	QDomText autoLoginText;
	if (user.autoLogin()) {
		autoLoginText = doc.createTextNode("true");
	} else {
		autoLoginText = doc.createTextNode("false");
	}
	autoLoginTag.appendChild(autoLoginText);

	return userTag;
}
