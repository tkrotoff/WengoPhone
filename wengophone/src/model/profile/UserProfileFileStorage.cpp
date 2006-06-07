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

#include "UserProfileFileStorage.h"

#include <model/contactlist/ContactListFileStorage.h>
#include <model/profile/UserProfileXMLSerializer.h>
#include <model/profile/UserProfile.h>
#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <util/File.h>
#include <util/Logger.h>

using namespace std;

static const std::string USERPROFILE_FILENAME = "userprofile.xml";

UserProfileFileStorage::UserProfileFileStorage(UserProfile & userProfile)
	: UserProfileStorage(userProfile) {
}

UserProfileFileStorage::~UserProfileFileStorage() {
}

bool UserProfileFileStorage::load(const std::string & url) {
	if (!loadProfile(url)) {
		return false;
	}

	IMAccountHandlerFileStorage imAccountHandlerFileStorage(*(_userProfile._imAccountHandler));
	imAccountHandlerFileStorage.load(url);

	ContactListFileStorage contactListFileStorage(_userProfile._contactList, *(_userProfile._imAccountHandler));
	contactListFileStorage.load(url);

	return true;
}

bool UserProfileFileStorage::save(const std::string & url) {
	File::createPath(url);

	if (!saveProfile(url)) {
		return false;
	}

	IMAccountHandlerFileStorage imAccountHandlerFileStorage(*(_userProfile._imAccountHandler));
	imAccountHandlerFileStorage.save(url);

	ContactListFileStorage contactListFileStorage(_userProfile._contactList, *(_userProfile._imAccountHandler));
	contactListFileStorage.save(url);

	return true;
}

bool UserProfileFileStorage::loadProfile(const std::string & url) {
	FileReader file(url + USERPROFILE_FILENAME);
	if (file.open()) {
		string data = file.read();
		file.close();

		UserProfileXMLSerializer serializer(_userProfile);
		serializer.unserialize(data);
		return true;
	}

	return true;
}

bool UserProfileFileStorage::saveProfile(const std::string & url) {
	FileWriter file(url + USERPROFILE_FILENAME);
	UserProfileXMLSerializer serializer(_userProfile);

	file.write(serializer.serialize());
	file.close();
	return true;
}
