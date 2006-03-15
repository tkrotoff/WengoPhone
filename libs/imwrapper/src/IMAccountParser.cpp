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

#include <imwrapper/IMAccountParser.h>

#include <imwrapper/IMAccount.h>

#include <tinyxml.h>

using namespace std;

IMAccountParser::IMAccountParser(IMAccount & imAccount, const std::string & data) 
: _imAccount(imAccount), _data(data) {
}

bool IMAccountParser::parse() {
	TiXmlDocument doc;
	EnumIMProtocol imProtocol;

	doc.Parse(_data.c_str());
	
	TiXmlHandle docHandle(&doc);
	TiXmlHandle account = docHandle.FirstChild("account");
	
	// Retrieving protocol
	TiXmlElement * lastChildElt = account.Element();
	if (lastChildElt) {
		_imAccount._protocol = imProtocol.toIMProtocol(lastChildElt->Attribute("protocol"));
	} else {
		return false;
	}

	//Retrieving login
	_imAccount._login = account.FirstChild("login").FirstChild().Text()->Value();
	
	//Retrieving password
	_imAccount._password = account.FirstChild("password").FirstChild().Text()->Value();

	return true;
}
