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

#include "ContactListFileStorage.h"

#include "ContactListXMLSerializer.h"
#include "ContactList.h"
#include "ContactGroup.h"
#include "Contact.h"

#include <util/File.h>
#include <util/Logger.h>

#include <string>
using namespace std;

ContactListFileStorage::ContactListFileStorage(ContactList & contactList, IMAccountHandler & imAccountHandler)
	: ContactListStorage(contactList), _imAccountHandler(imAccountHandler) {
}

ContactListFileStorage::~ContactListFileStorage() {
}

bool ContactListFileStorage::load(const std::string & url) {
	FileReader file(url);
	if (file.open()) {
		string data = file.read();
		file.close();

		ContactListXMLSerializer serializer(_contactList, _imAccountHandler);
		serializer.unserialize(data);

		LOG_DEBUG("file contactlist.xml loaded");
		return true;
	}

	return false;
}

bool ContactListFileStorage::save(const std::string & url) {
	FileWriter file(url);
	ContactListXMLSerializer serializer(_contactList, _imAccountHandler);

	file.write(serializer.serialize());
	file.close();
	
	return true;
}
