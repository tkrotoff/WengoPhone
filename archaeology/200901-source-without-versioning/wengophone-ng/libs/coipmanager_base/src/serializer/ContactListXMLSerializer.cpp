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

#include <coipmanager_base/serializer/ContactListXMLSerializer.h>

#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/serializer/ContactXMLSerializer.h>

#include <tinyxml.h>

ContactListXMLSerializer::ContactListXMLSerializer(ContactList & contactList)
	: _contactList(contactList) {
}

std::string ContactListXMLSerializer::serialize() {
	std::string result;

	result += "<contactlist>\n";

	for (ContactList::iterator it = _contactList.begin();
		it != _contactList.end();
		++it) {
		ContactXMLSerializer serializer(*it);
		result += serializer.serialize();
	}

	result += "</contactlist>\n";

	return result;
}

bool ContactListXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	bool result = true;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlNode * contactlist = docHandle.FirstChild("contactlist").Node();

	if (contactlist) {
		//Retrieving Contacts
		TiXmlNode * lastChild = NULL;
		while (result && (lastChild = contactlist->IterateChildren("wgcard", lastChild))) {
			std::string nodeData;
			nodeData << *lastChild;
			Contact contact;
			ContactXMLSerializer serializer(contact);
			result = serializer.unserialize(nodeData);
			_contactList.push_back(contact);
		}
		////
	}

	return result;
}
