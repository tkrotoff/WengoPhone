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

#include <coipmanager_base/serializer/ContactXMLSerializer.h>

#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/imcontact/IMContactList.h>
#include <coipmanager_base/serializer/IMContactXMLSerializer.h>

#include <serializer/StringListXMLSerializer.h>
#include <util/String.h>

#include <tinyxml.h>

ContactXMLSerializer::ContactXMLSerializer(Contact & contact)
	: ProfileXMLSerializer(contact),
	_contact(contact) {
}

std::string ContactXMLSerializer::serialize() {
	std::string result;

	result += "<wgcard version=\"1.0\" xmlns=\"http://www.openwengo.org/wgcard/1.0\">\n";

	result += ProfileXMLSerializer::serialize();

	//Serializing IMContacts
	IMContactList imContactList = _contact.getIMContactList();
	for (IMContactList::iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		IMContactXMLSerializer imContactSerializer(*it);
		result += imContactSerializer.serialize();
	}
	////

	//Serializing group
	StringList stringList = _contact.getGroupList();
	StringListXMLSerializer stringListSerializer(stringList);
	result += ("<groups>" + stringListSerializer.serialize() + "</groups>");
	////

	result += "</wgcard>\n";

	return result;
}

bool ContactXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	bool result = true;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle wgCard = docHandle.FirstChild("wgcard");

	ProfileXMLSerializer::unserializeContent(wgCard);

	//Retrieving Groups
	TiXmlNode * groups = wgCard.FirstChild("groups").Node();

	if (groups) {
		std::string nodeData;
		nodeData << *(groups->FirstChild());

		StringList stringList;
		StringListXMLSerializer stringListSerializer(stringList);
		stringListSerializer.unserialize(nodeData);

		for (StringList::const_iterator it = stringList.begin();
			it != stringList.end();
			++it) {
			_contact.addToGroup(*it);
		}
	}
	////

	//Retrieving IMContacts
	TiXmlNode * imLastChild = NULL;
	while (result && (imLastChild = wgCard.Node()->IterateChildren("im", imLastChild))) {
		std::string imData;
		IMContact imContact;
		IMContactXMLSerializer imContactSerializer(imContact);

		imData << *imLastChild;
		result = imContactSerializer.unserialize(imData);

		if (result) {
			_contact.addIMContact(imContact);
		}
	}
	////

	return result;
}
