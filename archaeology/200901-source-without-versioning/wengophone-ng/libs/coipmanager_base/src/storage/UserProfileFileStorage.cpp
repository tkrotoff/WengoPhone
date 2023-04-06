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

#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <coipmanager_base/serializer/UserProfileXMLSerializer.h>
#include <coipmanager_base/storage/AccountListFileStorage.h>
#include <coipmanager_base/storage/ContactGroupListFileStorage.h>
#include <coipmanager_base/storage/ContactListFileStorage.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/String.h>

#include <tinyxml.h>

static const std::string USERPROFILE_FILENAME = "userprofile.xml";
static const std::string PROFILES_DIR = "profiles/";
static const std::string BACKUPS_DIR = "backups/";
QMutex * UserProfileFileStorage::_mutex = new QMutex();
std::string UserProfileFileStorage::_pathToProfiles;

UserProfileFileStorage::UserProfileFileStorage(const std::string & pathToProfiles) {
	_pathToProfiles = pathToProfiles;

	if (!_pathToProfiles.empty()) {
		File::createPath(File::convertPathSeparators(_pathToProfiles + BACKUPS_DIR));
	}

	// Removing .new directories
	File path(_pathToProfiles + PROFILES_DIR);
	StringList directories = path.getDirectoryList();
	for (StringList::const_iterator it = directories.begin();
		it != directories.end();
		++it) {
		if (String((*it)).endsWith(".new")) {
			File dirFullPath(_pathToProfiles + PROFILES_DIR + (*it));
			dirFullPath.remove();
		}
	}
	////
}

UserProfileFileStorage::~UserProfileFileStorage() {
}

std::string UserProfileFileStorage::getProfilePath(const std::string & profileId) {
	return File::convertPathSeparators(_pathToProfiles + PROFILES_DIR + profileId + "/");
}

std::string UserProfileFileStorage::getTempProfilePath(const std::string & profileId) {
	return File::convertPathSeparators(_pathToProfiles + PROFILES_DIR + profileId + ".new/");
}

std::string UserProfileFileStorage::getBackupProfilePath(const std::string & profileId) {
	return File::convertPathSeparators(_pathToProfiles + BACKUPS_DIR + profileId + "/");
}

UserProfileFileStorage::UserProfileStorageError UserProfileFileStorage::load(const std::string & profileId, UserProfile & userProfile) {
	QMutexLocker lock(_mutex);

	UserProfileStorageError result = UserProfileStorageErrorCannotBeLoaded;

	if (!_pathToProfiles.empty()) {
		if (!loadFromProfiles(profileId, userProfile)) {
			LOG_ERROR("cannot load profile " + profileId + " from 'profiles'. Trying from 'backups'.");
			File profileDir(getProfilePath(profileId));
			profileDir.remove();
			if (!loadFromBackups(profileId, userProfile)) {
				LOG_ERROR("cannot load profile " + profileId + " from 'backups'.");
				File backupDir(getBackupProfilePath(profileId));
				backupDir.remove();
				result = UserProfileStorageErrorCannotBeLoaded;
			} else {
				LOG_INFO("backup recovery successful");
				userProfile.setUUID(profileId);
				result = UserProfileStorageErrorLoadedFromBackup;
			}
		} else {
			LOG_INFO("profile " + profileId + " loaded successfully");
			userProfile.setUUID(profileId);
			result = UserProfileStorageErrorNoError;
		}
	}

	return result;
}

bool UserProfileFileStorage::loadFromProfiles(const std::string & profileId, UserProfile & userProfile) {
	return loadFromDir(getProfilePath(profileId), userProfile);
}

bool UserProfileFileStorage::loadFromBackups(const std::string & profileId, UserProfile & userProfile) {
	return loadFromDir(getBackupProfilePath(profileId), userProfile);
}

bool UserProfileFileStorage::loadFromDir(const std::string & path, UserProfile & userProfile) {
	if (!loadProfile(path, userProfile)) {
		LOG_ERROR("cannot load userprofile.xml: " + path);
		return false;
	}

	AccountListFileStorage accountListFileStorage;
	if (!accountListFileStorage.load(path, userProfile.getAccountList())) {
		LOG_ERROR("cannot load AccountList: " + path);
		return false;
	}

	ContactGroupListFileStorage contactGroupListStorage;
	if (!contactGroupListStorage.load(path, userProfile.getContactGroupList())) {
		LOG_ERROR("cannot load ContactGroupList: " + path);
		return false;
	}

	ContactListFileStorage contactListFileStorage;
	if (!contactListFileStorage.load(path, userProfile.getContactList())) {
		LOG_ERROR("cannot load ContactList: " + path);
		return false;
	}

/* TODO: reactive history serialization
	if (!_userProfile.loadHistory(path)) {
		return false;
	}
*/
	return true;
}

bool UserProfileFileStorage::save(UserProfile & userProfile) {
	QMutexLocker lock(_mutex);

	bool result = false;

	if (!_pathToProfiles.empty()) {
		std::string profileId = userProfile.getUUID();
		std::string path = getProfilePath(profileId);
		std::string newPath = getTempProfilePath(profileId);

		// Backuping the last save
		if (File::exists(path)) {
			File file(path);
			file.move(getBackupProfilePath(profileId), true);
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

		if (!saveProfile(newPath, userProfile)) {
			return false;
		}

		AccountListFileStorage accountListFileStorage;
		if (!accountListFileStorage.save(newPath, userProfile.getAccountList())) {
			return false;
		}

		ContactGroupListFileStorage contactGroupListFileStorage;
		if (!contactGroupListFileStorage.save(newPath, userProfile.getContactGroupList())) {
			return false;
		}

		ContactListFileStorage contactListFileStorage;
		if (!contactListFileStorage.save(newPath, userProfile.getContactList())) {
			return false;
		}

	/* TODO: reactivate history serialization
		if (!_userProfile.saveHistory(newPath)) {
			return false;
		}
	*/
		////

		// If successful move the 'dir.new' to 'dir'
		File file(newPath);
		file.move(getProfilePath(profileId));
		////

		result = true;
	}

	return result;
}

bool UserProfileFileStorage::loadProfile(const std::string & url, UserProfile & userProfile) {
	bool result = false;

	FileReader file(url + USERPROFILE_FILENAME);
	if (file.open()) {
		std::string data = file.read();
		file.close();

		UserProfileXMLSerializer serializer(userProfile);
		result = serializer.unserialize(data);
	}

	return result;
}

bool UserProfileFileStorage::saveProfile(const std::string & url, const UserProfile & userProfile) {
	FileWriter file(url + USERPROFILE_FILENAME);
	UserProfileXMLSerializer serializer(const_cast<UserProfile &>(userProfile));

	file.write(serializer.serialize());
	file.close();
	return true;
}

StringList UserProfileFileStorage::getUserProfileIds() const {
	File path(_pathToProfiles + PROFILES_DIR);
	StringList result = path.getDirectoryList();

	return result;
}

std::string UserProfileFileStorage::getNameOfUserProfile(const std::string & profileId) const {
	std::string result;

	FileReader file(getProfilePath(profileId) + USERPROFILE_FILENAME);
	if (file.open()) {
		std::string data = file.read();
		file.close();

		TiXmlDocument doc;
		doc.Parse(data.c_str());

		TiXmlHandle docHandle(&doc);
		TiXmlHandle userprofile = docHandle.FirstChild("userprofile");

		// Gets the UserProfile name
		TiXmlNode * userprofileName = userprofile.FirstChild("userprofileName").FirstChild().Node();
		if (userprofileName) {
			result = userprofileName->Value();
		}
	}

	return result;
}
