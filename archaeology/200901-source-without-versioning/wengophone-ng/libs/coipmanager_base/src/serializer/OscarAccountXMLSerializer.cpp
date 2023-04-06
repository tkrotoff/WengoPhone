/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include <coipmanager_base/serializer/OscarAccountXMLSerializer.h>

#include <coipmanager_base/account/OscarAccount.h>

#include <util/String.h>

#include <tinyxml.h>

OscarAccountXMLSerializer::OscarAccountXMLSerializer(OscarAccount & account) 
	: IAccountXMLSerializer(account) {
}

std::string OscarAccountXMLSerializer::serialize() {
	std::string result;
	
	OscarAccount & myAccount = (OscarAccount &) _account;

	result += "<oscar.server>" + myAccount.getServer() + "</oscar.server>";
	result += "<oscar.port>" + String::fromNumber(myAccount.getServerPort()) + "</oscar.port>";
	result += "<oscar.encoding>" + myAccount.getEncoding() + "</oscar.encoding>";

	return result;
}

bool OscarAccountXMLSerializer::unserialize(TiXmlHandle & rootElt) {
	OscarAccount & myAccount = (OscarAccount &) _account;

	// Retrieving server
	TiXmlNode * server = rootElt.FirstChild("oscar.server").FirstChild().Node();
	if (server) {
		myAccount.setServer(server->Value());
	}
	////

	// Retrieving server port
	TiXmlNode * port = rootElt.FirstChild("oscar.port").FirstChild().Node();
	if (port) {
		String serverPort = port->Value();
		myAccount.setServerPort(serverPort.toInteger());
	}
	////

	// Retrieving encoding
	TiXmlNode * encoding = rootElt.FirstChild("oscar.encoding").FirstChild().Node();
	if (encoding) {
		myAccount.setEncoding(encoding->Value());
	}
	////

	return true;
}
