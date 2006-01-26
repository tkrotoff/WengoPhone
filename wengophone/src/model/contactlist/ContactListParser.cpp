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

#include "ContactListParser.h"

#include "ContactList.h"
#include "ContactGroup.h"
#include "model/WengoPhoneLogger.h"

#include <tinyxml.h>

#include <iostream>
using namespace std;

ContactListParser::ContactListParser(ContactList & contactList, const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());

	TiXmlHandle docHandle(& doc);

	TiXmlNode * list = docHandle.FirstChild("contactList").Node();
	if (!list) {
		return;
	}

	for (TiXmlNode * node = list->FirstChild("contactGroup"); node; node = node->NextSibling("contactGroup")) {
		TiXmlElement * elem = node->ToElement();
		if (elem) {
			std::string contactGroupName = elem->Attribute("name");
			ContactGroup * contactGroup = contactList[contactGroupName];
			if (!contactGroup) {
				contactGroup = new ContactGroup(contactGroupName);
			}
			contactList.addContactGroup(contactGroup);
			std::string tmp;
			tmp << *elem;
			contactGroup->unserialize(tmp);
		}
	}
}
