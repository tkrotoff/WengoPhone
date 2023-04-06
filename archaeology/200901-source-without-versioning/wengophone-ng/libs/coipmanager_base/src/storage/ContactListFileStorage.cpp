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

#include <coipmanager_base/storage/ContactListFileStorage.h>

#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/serializer/ContactListXMLSerializer.h>

#include <util/File.h>
#include <util/Logger.h>

#include <string>

static const std::string CONTACTLIST_FILENAME = "contactlist.xml";

ContactListFileStorage::ContactListFileStorage() {
}

ContactListFileStorage::~ContactListFileStorage() {
}

bool ContactListFileStorage::load(const std::string & url, ContactList & contactList) {
	FileReader file(url + CONTACTLIST_FILENAME);
	if (file.open()) {
		std::string data = file.read();
		file.close();

		ContactListXMLSerializer serializer(contactList);
		serializer.unserialize(data);
		return true;
	}

	return false;
}

bool ContactListFileStorage::save(const std::string & url, const ContactList & contactList) {
	FileWriter file(url + CONTACTLIST_FILENAME);
	ContactList & myContactList = const_cast<ContactList &>(contactList);
	ContactListXMLSerializer serializer(myContactList);

	file.write(serializer.serialize());
	file.close();
	return true;
}
