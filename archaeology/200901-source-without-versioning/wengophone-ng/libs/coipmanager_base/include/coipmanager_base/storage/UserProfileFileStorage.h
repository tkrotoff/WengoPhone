/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWUSERPROFILEFILESTORAGE_H
#define OWUSERPROFILEFILESTORAGE_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <util/StringList.h>

#include <QtCore/QMutex>

class UserProfile;

/**
 * Saves the UserProfile object and its dependencies.
 *
 * @ingroup coipmanager
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API UserProfileFileStorage {
public:

	enum UserProfileStorageError {
		/** The desired UserProfile has been loaded successfully. */
		UserProfileStorageErrorNoError,

		/** The given UserProfile does not exists. */
		UserProfileStorageErrorDoesNotExist,

		/** The desired UserProfile has been loaded from backups. */
		UserProfileStorageErrorLoadedFromBackup,

		/** The desired UserProfile cannot be loaded. */
		UserProfileStorageErrorCannotBeLoaded,

	};

	/**
	 * Gets the full path to the profile of id 'profileId'.
	 */
	static std::string getProfilePath(const std::string & profileId);

	/**
	 * Construct the UserProfileFileStorage class.
	 *
	 * @param pathToProfiles path to where profiles 'are'/'must be' saved
	 */
	UserProfileFileStorage(const std::string & pathToProfiles);

	~UserProfileFileStorage();

	/**
	 * Gets the ids of existing user profiles.
	 *
	 * These UserProfiles can then be loaded with getUserProfile.
	 *
	 * Reentrant
	 *
	 * @return vector of UserProfiles
	 */
	StringList getUserProfileIds() const;

	/**
	 * Gets the name of the UserProfile with id 'profileId'.
	 *
	 */
	std::string getNameOfUserProfile(const std::string & profileId) const;

	/**
	 * Will load the profile with id 'profileId'.
	 *
	 * If an error occured while trying the profile,
	 * the method will try to load the profile from
	 * the backup directory.
	 *
	 * ThreadSafe
	 *
	 */
	UserProfileStorageError load(const std::string & profileId, UserProfile & userProfile);

	/**
	 * Saves the given UserProfile.
	 *
	 * ThreadSafe.
	 *
	 */
	bool save(UserProfile & userProfile);

private:

	/**
	 * Gets the temporary save profile dir.
	 */
	static std::string getTempProfilePath(const std::string & profileId);

	/**
	 * Gets the full path to the backup profile named 'profileName'.
	 */
	static std::string getBackupProfilePath(const std::string & profileId);

	/**
	 * Tries to load a profile from the 'profiles' dir.
	 *
	 * @return false if error, true otherwise
	 */
	bool loadFromProfiles(const std::string & profileId, UserProfile & userProfile);

	/**
	 * Tries to load a profile from the 'backups' dir.
	 *
	 * @return false if error, true otherwise
	 */
	bool loadFromBackups(const std::string & profileId, UserProfile & userProfile);

	/**
	 * Loads a profile from a dir.
	 *
	 * @return false if error, true otherwise
	 */
	bool loadFromDir(const std::string & path, UserProfile & userProfile);

	/**
	 * Loads the UserProfile object from url/userprofile.xml
	 *
	 * @return false if error, true otherwise
	 */
	bool loadProfile(const std::string & url, UserProfile & userProfile);

	/**
	 * Saves the UserProfile object in url/userprofile.xml
	 *
	 * @return false if error, true otherwise
	 */
	bool saveProfile(const std::string & url, const UserProfile & userProfile);

	/** For thread-safe operations. */
	static QMutex * _mutex;

	/** Path to profiles. */
	static std::string _pathToProfiles;
};

#endif	//OWUSERPROFILEFILESTORAGE_H
