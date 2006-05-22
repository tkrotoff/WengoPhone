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

#include "ContactXMLSerializer.h"

#include "Contact.h"
#include "ContactGroup.h"
#include "ContactList.h"

#include <model/profile/ProfileXMLSerializer.h>

#include <imwrapper/IMContactXMLSerializer.h>
#include <imwrapper/IMContactSet.h>

#include <tinyxml.h>

using namespace std;

ContactXMLSerializer::ContactXMLSerializer(Contact & contact, 
	ContactList & contactList, IMAccountHandler & imAccountHandler)
: ProfileXMLSerializer(contact), _contact(contact), _contactList(contactList), _imAccountHandler(imAccountHandler) {
}

string ContactXMLSerializer::serialize() {
	string result;

	result += "<wgcard version=\"1.0\" xmlns=\"http://www.openwengo.org/wgcard/1.0\">\n";

	result += ProfileXMLSerializer::serialize();

	// Serializing IMContacts
	for (IMContactSet::const_iterator it = _contact._imContactSet.begin();
		it != _contact._imContactSet.end();
		++it) {
		IMContactXMLSerializer imContactSerializer((IMContact &)*it, _imAccountHandler);
		result += imContactSerializer.serialize();
	}
	////

	// Serializing group
	_contactList.lock();
	ContactGroup * contactGroup = _contactList.getContactGroup(_contact.getGroupId());
	if (contactGroup) {
		result += "<group><![CDATA[" + contactGroup->getName() + "]]></group>\n";
	}
	_contactList.unlock();
	////

	result += "</wgcard>\n";

	return result;
}

bool ContactXMLSerializer::unserialize(const string & data) {
	TiXmlDocument doc;
	EnumIMProtocol imProtocol;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle wgCard = docHandle.FirstChild("wgcard");

	ProfileXMLSerializer::unserializeContent(wgCard);

	//Retrieving Groups
	TiXmlNode * group = wgCard.FirstChild("group").Node();
	if (group) {
		_contactList._addToContactGroup(string(group->FirstChild()->Value()), _contact);
	}
	////

	//Retrieving IMContacts
	TiXmlNode * imLastChild = NULL;
	while ((imLastChild = wgCard.Node()->IterateChildren("im", imLastChild))) {
		string imData;
		IMContact imContact(EnumIMProtocol::IMProtocolUnknown, "");
		IMContactXMLSerializer imContactSerializer(imContact, _imAccountHandler);

		imData << *imLastChild;
		imContactSerializer.unserialize(imData);

		_contact._addIMContact(imContact);
	}
	////

	return true;
}
