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

#include "ContactListXMLLayer.h"

#include "ContactList.h"
#include "ContactGroup.h"
#include "Contact.h"

#include <File.h>
#include <Logger.h>

#include <string>
using namespace std;

ContactListXMLLayer::ContactListXMLLayer(ContactList & contactList)
	: ContactListDataLayer(contactList) {
	load();
}

ContactListXMLLayer::~ContactListXMLLayer() {
	save();
}

bool ContactListXMLLayer::load() {
	FileReader file("contactlist.xml");
	if (file.open()) {
		string data = file.read();
		file.close();

		_contactList.unserialize(data);

		LOG_DEBUG("file contactlist.xml loaded");
		return true;
	}

	return false;
}

bool ContactListXMLLayer::save() {
	FileWriter file("contactlist.xml");
	file.write(_contactList.serialize());
	file.close();
	
	return true;
}
