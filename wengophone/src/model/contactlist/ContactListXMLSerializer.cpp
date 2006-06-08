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

#include "ContactListXMLSerializer.h"

#include "ContactXMLSerializer.h"
#include "ContactList.h"
#include "Contact.h"

#include <tinyxml.h>

using namespace std;

ContactListXMLSerializer::ContactListXMLSerializer(ContactList & contactList, IMAccountHandler & imAccountHandler)
	: _contactList(contactList),
	_imAccountHandler(imAccountHandler) {
}

string ContactListXMLSerializer::serialize() {
	string result;

	result += "<contactlist>\n";

	for (ContactList::Contacts::const_iterator it = _contactList._contacts.begin();
		it != _contactList._contacts.end();
		++it) {
		ContactXMLSerializer serializer((Contact &)(*it), _contactList,
			_imAccountHandler);
		result += serializer.serialize();
	}

	result += "</contactlist>\n";

	return result;
}

bool ContactListXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlNode * contactlist = docHandle.FirstChild("contactlist").Node();

	if (contactlist) {
		//Retrieving Contacts
		TiXmlNode * lastChild = NULL;
		while ((lastChild = contactlist->IterateChildren("wgcard", lastChild))) {
			string nodeData;
			nodeData << *lastChild;
			ContactXMLSerializer serializer(_contactList.createContact(),
				_contactList, _imAccountHandler);
			serializer.unserialize(nodeData);
		}
		////
	}

	return true;
}
