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

#include "UserProfileHandler.h"

#include <model/account/wengo/WengoAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/profile/UserProfile.h>
#include <model/profile/UserProfileFileStorage.h>
#include <model/webservices/url/WsUrl.h>

#include <util/File.h>
#include <util/Logger.h>

UserProfileHandler::UserProfileHandler() {
	_currentUserProfile = NULL;
	_desiredUserProfile = NULL;
	_importDefaultProfileToProfile = false;
	_saveTimer.lastTimeoutEvent +=
		boost::bind(&UserProfileHandler::saveTimerLastTimeoutEventHandler, this, _1);
}

UserProfileHandler::~UserProfileHandler() {
	if (_desiredUserProfile) {
		delete _desiredUserProfile;
	}

	if (_currentUserProfile) {
		_currentUserProfile->disconnect();
		saveUserProfile(*_currentUserProfile);
		setLastUsedUserProfile(*_currentUserProfile);
		WsUrl::setWengoAccount(NULL);
		delete _currentUserProfile;
	}
}

void UserProfileHandler::save() {
	if (_currentUserProfile) {
		saveUserProfile(*_currentUserProfile);
		setLastUsedUserProfile(*_currentUserProfile);
	}
}

StringList UserProfileHandler::getUserProfileNames() {
	StringList result;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	File path(File::convertPathSeparators(config.getConfigDir() + "profiles/"));
	result = path.getDirectoryList();

	return result;
}

UserProfile * UserProfileHandler::getUserProfile(const std::string & name) {
	UserProfile * result = NULL;
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (userProfileExists(name)) {
		result = new UserProfile();
		UserProfileFileStorage userProfileStorage(*result);
		userProfileStorage.load(File::convertPathSeparators(config.getConfigDir() + "profiles/" + name + "/"));
	}

	return result;
}

UserProfileHandler::UserProfileHandlerError UserProfileHandler::createUserProfile(const WengoAccount & wengoAccount) {
	UserProfile * userProfile = NULL;
	UserProfileHandlerError result = UserProfileHandlerErrorNoError;
	std::string profileName;

	userProfile = new UserProfile();
	if (!wengoAccount.getWengoLogin().empty()) {
		userProfile->setWengoAccount(wengoAccount);
	}

	profileName = userProfile->getName();

	if (!userProfileExists(profileName)) {
		if (userProfile->isWengoAccountValid()) {
			saveUserProfile(*userProfile);
			if ((profileName != UserProfile::DEFAULT_USERPROFILE_NAME) &&
				userProfileExists(UserProfile::DEFAULT_USERPROFILE_NAME)) {
				defaultUserProfileExistsEvent(*this, profileName);
			}
			result = UserProfileHandlerErrorNoError;
		} else {
			wengoAccountNotValidEvent(*this, *userProfile->getWengoAccount());
			result = UserProfileHandlerErrorWengoAccountNotValid;
		}
	} else {
		LOG_ERROR("A UserProfile with the name: " + profileName + " already exists");
		result = UserProfileHandlerErrorUserProfileAlreadyExists;
	}

	delete userProfile;

	return result;
}

void UserProfileHandler::createAndSetUserProfile(const WengoAccount & wengoAccount) {
	if (createUserProfile(wengoAccount) != UserProfileHandlerErrorWengoAccountNotValid) {
		std::string profileName = wengoAccount.getWengoLogin();
		if (profileName.empty()) {
			profileName = UserProfile::DEFAULT_USERPROFILE_NAME;
		}
		setCurrentUserProfile(profileName, wengoAccount);
	}
}

bool UserProfileHandler::userProfileExists(const std::string & name) {
	bool result = false;
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string path = File::convertPathSeparators(config.getConfigDir() + "profiles/" + name + "/");

	if (!name.empty() && File::exists(path)) {
		result = true;
	}

	return result;
}

void UserProfileHandler::setCurrentUserProfile(const std::string & name,
	const WengoAccount & wengoAccount) {

	Mutex::ScopedLock lock(_mutex);

	UserProfile * result = getUserProfile(name);

	// Check if the desired UserProfile is different from the current UserProfile
	// and check if the WengoAccount of the current UserProfile is different from the given WengoAccount
	// (if so the WengoAccount (only the password and keep password members) will be updated).
	if (!_currentUserProfile ||
		(_currentUserProfile &&
			((_currentUserProfile->getName() != name) ||
			((wengoAccount.getWengoPassword() != result->getWengoAccount()->getWengoPassword()) ||
					(wengoAccount.hasAutoLogin() != result->getWengoAccount()->hasAutoLogin()))))) {

		if (result) {
			// If the WengoAccount is not empty, we update the one in UserProfile
			if (result->hasWengoAccount() &&
				!wengoAccount.getWengoLogin().empty() &&
				((wengoAccount.getWengoPassword() != result->getWengoAccount()->getWengoPassword()) ||
					(wengoAccount.hasAutoLogin() != result->getWengoAccount()->hasAutoLogin()))) {
				WengoAccount myWengoAccount(result->getWengoAccount()->getWengoLogin(),
					wengoAccount.getWengoPassword(), wengoAccount.hasAutoLogin());
				result->setWengoAccount(myWengoAccount);
			}

			if (_currentUserProfile) {
				LOG_DEBUG("UserProfile will change");
				_desiredUserProfile = result;
				currentUserProfileWillDieEvent(*this);
			} else {
				LOG_DEBUG("No current UserProfile set. Change now");
				_currentUserProfile = result;
				initializeCurrentUserProfile();
			}
		} else {
			if (_currentUserProfile && name.empty()) {
				_desiredUserProfile = NULL;
				currentUserProfileWillDieEvent(*this);
			} else {
				noCurrentUserProfileSetEvent(*this);
			}
		}
	}
}

void UserProfileHandler::setLastUsedUserProfile(const UserProfile & userProfile) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::PROFILE_LAST_USED_NAME_KEY, userProfile.getName());
}

void UserProfileHandler::currentUserProfileReleased() {
	if (_currentUserProfile) {
		saveUserProfile(*_currentUserProfile);
		WsUrl::setWengoAccount(NULL);
		delete _currentUserProfile;
		_currentUserProfile = NULL;
	}

	if (_importDefaultProfileToProfile) {
		actuallyImportDefaultProfileToProfile();
	} else if (_desiredUserProfile) {
		// If we want to change the UserProfile
		LOG_DEBUG("Old UserProfile killed. Setting the new one");
		_currentUserProfile = _desiredUserProfile;
		_desiredUserProfile = NULL;
		initializeCurrentUserProfile();
	}
}

void UserProfileHandler::initializeCurrentUserProfile() {
	_currentUserProfile->profileChangedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactGroupAddedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactGroupRemovedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactGroupRenamedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactAddedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactRemovedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactMovedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);
	_currentUserProfile->getContactList().contactChangedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this);

	_currentUserProfile->init();

	if (_currentUserProfile->hasWengoAccount()
		&& !_currentUserProfile->getWengoAccount()->isValid()
		&& !_currentUserProfile->isWengoAccountValid()) {
		WengoAccount wengoAccount = *_currentUserProfile->getWengoAccount();
		delete _currentUserProfile;
		_currentUserProfile = NULL;
		wengoAccountNotValidEvent(*this, wengoAccount);
	} else {
		WsUrl::setWengoAccount(_currentUserProfile->getWengoAccount());

		userProfileInitializedEvent(*this, *_currentUserProfile);

		_currentUserProfile->connect();

		setLastUsedUserProfile(*_currentUserProfile);
	}
}

void UserProfileHandler::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string profileName = config.getProfileLastUsedName();

	setCurrentUserProfile(profileName, WengoAccount::empty);
}

void UserProfileHandler::saveUserProfile(UserProfile & userProfile) {
	Config config = ConfigManager::getInstance().getCurrentConfig();
	UserProfileFileStorage userProfileStorage(userProfile);
	userProfileStorage.save(File::convertPathSeparators(config.getConfigDir()
		+ "profiles/" + userProfile.getName() + "/"));
}

void UserProfileHandler::profileChangedEventHandler() {
	if (!_saveTimerRunning) {
		_saveTimerRunning = true;
		_saveTimer.start(5000, 5000, 1);
	}
}

void UserProfileHandler::saveTimerLastTimeoutEventHandler(Timer & sender) {
	saveUserProfile(*_currentUserProfile);
	_saveTimerRunning = false;
}

void UserProfileHandler::importDefaultProfileToProfile(const std::string & profileName) {
	Mutex::ScopedLock lock(_mutex);

	_nameOfProfileToImport = profileName;

	if (_currentUserProfile) {
		_importDefaultProfileToProfile = true;
		currentUserProfileWillDieEvent(*this);
	} else {
		actuallyImportDefaultProfileToProfile();
	}
}

void UserProfileHandler::actuallyImportDefaultProfileToProfile() {
	UserProfile * defaultUserProfile = getUserProfile(UserProfile::DEFAULT_USERPROFILE_NAME);
	UserProfile * newUserProfile = getUserProfile(_nameOfProfileToImport);

	if (defaultUserProfile && newUserProfile && newUserProfile->hasWengoAccount()) {
		// Setting the Wengo account of the 'Default' UserProfile to the one
		// of the new UserProfile.
		defaultUserProfile->setWengoAccount(*newUserProfile->getWengoAccount());
		delete newUserProfile; newUserProfile = NULL;

		// Saving the old 'Default' UserProfile. It will actually overwrite
		// the new UserProfile and thus saves the ContactList and the IMAccounts
		// of the 'Default' UserProfile to the new one.
		saveUserProfile(*defaultUserProfile);

		// Deleting the 'Default' UserProfile directory
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		File defaultUserProfileDir(File::convertPathSeparators(config.getConfigDir()
			+ "profiles/" + UserProfile::DEFAULT_USERPROFILE_NAME + "/"));
		defaultUserProfileDir.remove();

		// Sets the current UserProfile to the new one
		setCurrentUserProfile(_nameOfProfileToImport, WengoAccount::empty);
	}

	_importDefaultProfileToProfile = false;
}
