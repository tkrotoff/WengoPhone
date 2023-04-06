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

#include <coipmanager_base/serializer/ContactGroupListXMLSerializer.h>

#include <coipmanager_base/contact/ContactGroupList.h>

#include <util/Logger.h>

#include <tinyxml.h>

ContactGroupListXMLSerializer::ContactGroupListXMLSerializer(ContactGroupList & contactGroupList)
	: _contactGroupList(contactGroupList) {
}

std::string ContactGroupListXMLSerializer::serialize() {
	std::string result;

	result += "<contactgrouplist>\n";

	for (ContactGroupList::iterator it = _contactGroupList.begin() ; it != _contactGroupList.end() ; it++) {	
		result += "<group>\n";
		result += "<uuid>" + (*it).first + "</uuid>\n";
		result += "<name>" + (*it).second + "</name>\n";
		result += "</group>\n";
	}

	result += "</contactgrouplist>\n";

	return result;	
}

bool ContactGroupListXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlNode * contactgroups = docHandle.FirstChild("contactgrouplist").Node();
	
	if (contactgroups) {
		TiXmlNode * lastChild = NULL;
		while ((lastChild = contactgroups->IterateChildren("group", lastChild))) {
			TiXmlHandle handle = TiXmlHandle(lastChild);
			std::string myUuid;
			TiXmlNode * uuid = handle.FirstChild("uuid").FirstChild().Node();
			if (uuid) {
				myUuid = uuid->Value();
			} else {
				LOG_ERROR("cannot get uuid");
				continue;
			}
		
			std::string myName;
			TiXmlNode * name = handle.FirstChild("name").FirstChild().Node();
			if (name) {
				myName = name->Value();
			} else {
				LOG_ERROR("cannot get name");
				continue;
			}

			_contactGroupList[myUuid] = myName;
		}

		return true;
	}

	return false;
}
