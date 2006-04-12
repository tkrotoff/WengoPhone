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

#include <imwrapper/IMContactXMLSerializer.h>

#include <imwrapper/IMContact.h>
#include <imwrapper/IMAccountHandler.h>

#include <util/Logger.h>
#include <util/Base64.h>

#include <tinyxml.h>

using namespace std;

IMContactXMLSerializer::IMContactXMLSerializer(IMContact & imContact, IMAccountHandler & imAccountHandler)
: _imContact(imContact), _imAccountHandler(imAccountHandler) {
}

std::string IMContactXMLSerializer::serialize() {
	string result;
	EnumIMProtocol::IMProtocol protocol;

	result += ("<im protocol=\"" 
		+ EnumIMProtocol::toString(_imContact.getProtocol())
		+ "\">\n");

	result += ("<id>" + _imContact.getContactId() + "</id>\n");

	result += ("<alias>" + _imContact.getAlias() + "</alias>\n");

	if (_imContact.getIMAccount()) {
		result += ("<account>" + _imContact.getIMAccount()->getLogin() + "</account>\n");	
	}

	if (!_imContact._icon.getData().empty()) {
		result += ("<photo><![CDATA[" + Base64::encode(_imContact._icon.getData()) + "]]></photo>");
	}

	result += "<groups>\n";
	for (IMContact::GroupSet::const_iterator it = _imContact._groupSet.begin();
		it != _imContact._groupSet.end();
		++it) {
		result += ("<group>" + (*it) + "</group>\n");
	}
	result += "</groups>\n";

	result += "</im>\n";

	return result;
}

bool IMContactXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle im = docHandle.FirstChild("im");

	// Retrieving associated account
	EnumIMProtocol::IMProtocol protocol;

	TiXmlElement * lastChildElt = im.Element();
	if (lastChildElt) {
		protocol = EnumIMProtocol::toIMProtocol(lastChildElt->Attribute("protocol"));
	} else {
		return false;
	}

	TiXmlText * login = im.FirstChild("account").FirstChild().Text();
	if (login) {
		IMAccount account(login->Value(), "", protocol);
		//Find this IMAccount in IMAccountHandler
		IMAccountHandler::const_iterator it = _imAccountHandler.find(account);
		if (it != _imAccountHandler.end()) {
			_imContact._imAccount = (IMAccount *)&(*it);
		} else {
			LOG_FATAL("this IMAccount does not exist in IMAccountHandler: " + account.getLogin());
		}
		////
	} else {
		_imContact._imAccount = NULL;
		_imContact._protocol = protocol;
	}
	////

	//Retrieving contactId
	TiXmlText * contactId = im.FirstChild("id").FirstChild().Text();
	if (contactId) {
		_imContact._contactId = contactId->Value();
	}
	////

	// Retrieving alias
	TiXmlText * alias = im.FirstChild("alias").FirstChild().Text();
	if (alias) {
		_imContact._alias = alias->Value();
	}
	////

	// Retrieving icon
	TiXmlText * photo = im.FirstChild("photo").FirstChild().Text();
	if (photo) {
		Picture picture(Base64::decode(photo->Value()));
		_imContact.setIcon(picture);
	}
	////

	//Retrieving Groups
	TiXmlNode * groups = im.FirstChild("groups").Node();

	TiXmlNode * lastChild = NULL;
	while ((lastChild = groups->IterateChildren("group", lastChild))) {
		_imContact._groupSet.insert(string(lastChild->FirstChild()->Value()));
	}
	////

	return true;
}
