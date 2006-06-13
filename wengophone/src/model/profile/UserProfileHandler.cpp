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

UserProfileHandler::UserProfileHandler(WengoPhone & wengoPhone)
: _wengoPhone(wengoPhone) {
	_currentUserProfile = NULL;
	_desiredUserProfile = NULL;
}

UserProfileHandler::~UserProfileHandler() {
	if (_desiredUserProfile) {
		delete _desiredUserProfile;
	}

	if (_currentUserProfile) {
		_currentUserProfile->disconnect();
		saveUserProfile(*_currentUserProfile);
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

std::vector<std::string> UserProfileHandler::getUserProfileNames() {
	std::vector<std::string> result;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	File path(File::convertPathSeparators(config.getConfigDir() + "profiles/"));
	StringList list = path.getDirectoryList();

	result = list;

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

bool UserProfileHandler::createUserProfile(const WengoAccount & wengoAccount) {
	UserProfile * userProfile = NULL;
	bool result = false;
	std::string profileName;

	userProfile = new UserProfile();
	if (!wengoAccount.getWengoLogin().empty()) {
		userProfile->setWengoAccount(wengoAccount);
	}

	//FIXME: Here we could know if the WengoAccount is valid or not and then
	// save the UserProfile or not or do anything else
	profileName = userProfile->getName();

	if (!userProfileExists(profileName)) {
		if (userProfile->isWengoAccountValid()) {
			saveUserProfile(*userProfile);
			result = true;
		} else {
			wengoAccountNotValidEvent(*this, *userProfile->getWengoAccount());
		}
	} else {
		LOG_ERROR("A UserProfile with the name: " + profileName + " already exists");
	}

	delete userProfile;

	return result;
}

void UserProfileHandler::createAndSetUserProfile(const WengoAccount & wengoAccount) {
	if (createUserProfile(wengoAccount)) {
		std::string profileName = wengoAccount.getWengoLogin();
		if (profileName.empty()) {
			profileName = "Default";
		}
		setCurrentUserProfile(profileName);
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

void UserProfileHandler::setCurrentUserProfile(const std::string & name) {
	if (!_currentUserProfile || (_currentUserProfile && (_currentUserProfile->getName() != name))) {
		UserProfile * result = getUserProfile(name);

		if (result) {
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
			noCurrentUserProfileSetEvent(*this);
		}
	}
}

void UserProfileHandler::setLastUsedUserProfile(const UserProfile & userProfile) {
	Config config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::PROFILE_LAST_USED_NAME_KEY, userProfile.getName());
}

void UserProfileHandler::currentUserProfileReleased() {
	if (_currentUserProfile) {
		saveUserProfile(*_currentUserProfile);
		WsUrl::setWengoAccount(NULL);
		delete _currentUserProfile;
		_currentUserProfile = NULL;
	}

	// If we want to change the UserProfile
	if (_desiredUserProfile) {
		LOG_DEBUG("Old UserProfile killed. Setting the new one");
		_currentUserProfile = _desiredUserProfile;
		_desiredUserProfile = NULL;
		initializeCurrentUserProfile();
	}
}

void UserProfileHandler::initializeCurrentUserProfile() {
	_currentUserProfile->profileChangedEvent +=
		boost::bind(&UserProfileHandler::profileChangedEventHandler, this, _1);

	_currentUserProfile->init();
	WsUrl::setWengoAccount(_currentUserProfile->getWengoAccount());
	userProfileInitializedEvent(*this, *_currentUserProfile);

	_currentUserProfile->connect();

	setLastUsedUserProfile(*_currentUserProfile);
}

void UserProfileHandler::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string profileName = config.getProfileLastUsedName();

	setCurrentUserProfile(profileName);
}

void UserProfileHandler::saveUserProfile(UserProfile & userProfile) {
	Config config = ConfigManager::getInstance().getCurrentConfig();
	UserProfileFileStorage userProfileStorage(userProfile);
	userProfileStorage.save(File::convertPathSeparators(config.getConfigDir()
		+ "profiles/" + userProfile.getName() + "/"));
}

void UserProfileHandler::profileChangedEventHandler(Profile & sender) {
	saveUserProfile((UserProfile &) sender);
}
