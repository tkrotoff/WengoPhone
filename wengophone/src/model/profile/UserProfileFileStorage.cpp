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

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/contactlist/ContactListFileStorage.h>
#include <model/profile/UserProfileXMLSerializer.h>
#include <model/profile/UserProfile.h>
#include <imwrapper/IMAccountHandlerFileStorage.h>

#include <util/File.h>
#include <util/Logger.h>

using namespace std;

static const std::string USERPROFILE_FILENAME = "userprofile.xml";
static const std::string PROFILES_DIR = "profiles/";
static const std::string BACKUPS_DIR = "backups/";

UserProfileFileStorage::UserProfileFileStorage(UserProfile & userProfile)
	: UserProfileStorage(userProfile) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	File::createPath(File::convertPathSeparators(config.getConfigDir() + BACKUPS_DIR));
}

UserProfileFileStorage::~UserProfileFileStorage() {
}

std::string UserProfileFileStorage::getProfilePath(const std::string & profileName) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return File::convertPathSeparators(config.getConfigDir() + PROFILES_DIR + profileName + "/");
}

std::string UserProfileFileStorage::getTempProfilePath(const std::string & profileName) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return File::convertPathSeparators(config.getConfigDir() + PROFILES_DIR + profileName + ".new/");
}

std::string UserProfileFileStorage::getBackupProfilePath(const std::string & profileName) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return File::convertPathSeparators(config.getConfigDir() + BACKUPS_DIR + profileName + "/");
}

bool UserProfileFileStorage::load(const std::string & profileName) {
	bool result = false;

	if (!loadFromProfiles(profileName)) {
		LOG_ERROR("cannot load profile " + profileName + " from 'profiles'."
			" Trying from 'backups'.");
		File profileDir(getProfilePath(profileName));
		profileDir.remove();
		if (!loadFromBackups(profileName)) {
			LOG_ERROR("cannot load profile " + profileName + " from 'backups'.");
			profileCannotBeLoadedEvent(*this, profileName);
			File backupDir(getBackupProfilePath(profileName));
			backupDir.remove();
		} else {
			LOG_INFO("backup recovery successful");
			profileLoadedFromBackupsEvent(*this, profileName);
			result = true;
		}
	} else {
		LOG_INFO("profile " + profileName + " loaded successfully");
		result = true;
	}

	return result;
}

bool UserProfileFileStorage::loadFromProfiles(const std::string & profileName) {
	return loadFromDir(getProfilePath(profileName));
}

bool UserProfileFileStorage::loadFromBackups(const std::string & profileName) {
	return loadFromDir(getBackupProfilePath(profileName));
}

bool UserProfileFileStorage::loadFromDir(const std::string & path) {
	if (!loadProfile(path)) {
		LOG_ERROR("cannot loadProfile: " + path);
		return false;
	}

	IMAccountHandlerFileStorage imAccountHandlerFileStorage(*(_userProfile._imAccountHandler));
	if (!imAccountHandlerFileStorage.load(path)) {
		LOG_ERROR("cannot load IMAccountHandler: " + path);
		return false;
	}

	ContactListFileStorage contactListFileStorage(_userProfile._contactList, *(_userProfile._imAccountHandler));
	if (!contactListFileStorage.load(path)) {
		LOG_ERROR("cannot load ContactList: " + path);
		return false;
	}

	return true;
}

bool UserProfileFileStorage::save(const std::string & profileName) {
	std::string path = getProfilePath(profileName);
	std::string newPath = getTempProfilePath(profileName);

	// Backuping the last save
	if (File::exists(path)) {
		File file(path);
		file.move(getBackupProfilePath(profileName));
	}
	////

	// Removing a possible .new dir
	if (File::exists(newPath)) {
		File file(newPath);
		file.remove();
	}
	////

	// Saving profile to .new dir
	File::createPath(newPath);

	if (!saveProfile(newPath)) {
		return false;
	}

	IMAccountHandlerFileStorage imAccountHandlerFileStorage(*(_userProfile._imAccountHandler));
	if (!imAccountHandlerFileStorage.save(newPath)) {
		return false;	
	}

	ContactListFileStorage contactListFileStorage(_userProfile._contactList, *(_userProfile._imAccountHandler));
	if (!contactListFileStorage.save(newPath)) {
		return false;
	}
	////

	// If successful move the 'dir.new' to 'dir'
	File file(newPath);
	file.move(getProfilePath(profileName));
	////

	return true;
}

bool UserProfileFileStorage::loadProfile(const std::string & url) {
	bool result = false;

	FileReader file(url + USERPROFILE_FILENAME);
	if (file.open()) {
		string data = file.read();
		file.close();

		UserProfileXMLSerializer serializer(_userProfile);
		result = serializer.unserialize(data);
	}

	return result;
}

bool UserProfileFileStorage::saveProfile(const std::string & url) {
	FileWriter file(url + USERPROFILE_FILENAME);
	UserProfileXMLSerializer serializer(_userProfile);

	file.write(serializer.serialize());
	file.close();
	return true;
}
