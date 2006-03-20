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

#include <imwrapper/IMAccountHandlerXMLSerializer.h>

#include <imwrapper/IMAccountHandler.h>
#include <imwrapper/IMAccountXMLSerializer.h>
#include <imwrapper/IMAccount.h>

#include <tinyxml.h>

using namespace std;

IMAccountHandlerXMLSerializer::IMAccountHandlerXMLSerializer(IMAccountHandler & imAccountHandler)
: _imAccountHandler(imAccountHandler) {
}

std::string IMAccountHandlerXMLSerializer::serialize() {
	string result;

	result += "<imaccounts>\n";

	for (IMAccountHandler::const_iterator it = _imAccountHandler.begin() ; it != _imAccountHandler.end() ; it++) {
		IMAccountXMLSerializer serializer((IMAccount &)*it);
		result += serializer.serialize();
	}

	result += "</imaccounts>\n";

	return result;	
}

bool IMAccountHandlerXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlNode * imaccounts = docHandle.FirstChild("imaccounts").Node();
	
	TiXmlNode * lastChild = NULL;
	while ((lastChild = imaccounts->IterateChildren("account", lastChild))) {
		string accountData;
		accountData << *lastChild;
		
		IMAccount account;
		IMAccountXMLSerializer serializer(account);
		serializer.unserialize(accountData);
		_imAccountHandler.insert(account);
	}
	
	return true;
}
