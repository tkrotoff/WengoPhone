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

#include "UserProfileHandler.h"

#include <model/WengoPhone.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/ContactList.h>
#include <model/profile/UserProfile.h>
#include <model/profile/UserProfileFileStorage.h>
#include <model/webservices/url/WsUrl.h>

#include <imwrapper/IMWrapperFactory.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/ThreadEvent.h>

UserProfileHandler::UserProfileHandler() {
	_currentUserProfile = NULL;
	_desiredUserProfile = NULL;

	cleanupUserProfileDirectories();

	_saveTimerRunning = false;
	_autoSave = true;
	_saveTimer.lastTimeoutEvent +=
		boost::bind(&UserProfileHandler::saveTimerLastTimeoutEventHandler, this, _1);
}

UserProfileHandler::~UserProfileHandler() {
	OWSAFE_DELETE(_desiredUserProfile);

	{
		RecursiveMutex::ScopedLock lock(_mutex);
		_saveTimerRunning = false;
		_saveTimer.stop();
	}

	if (_currentUserProfile) {
		saveUserProfile(*_currentUserProfile);
		setLastUsedUserProfile(*_currentUserProfile);
		WsUrl::setWengoAccount(NULL);

		OWSAFE_DELETE(_currentUserProfile);

		uninitExtLibraries();
	}
}

StringList UserProfileHandler::getUserProfileNames(bool onlyRealDir) {
	StringList result;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	File path(File::convertPathSeparators(config.getConfigDir() + "profiles/"));
	result = path.getDirectoryList();

	if (onlyRealDir) {
		StringList::iterator begin = result.begin();
		for (int interator = result.size(); --interator >= 0; ) {
			if (String(result[interator]).endsWith(".new") || String(result[interator]).endsWith(".old")) {
				result.erase(begin + interator);
			}
		}
	}

	return result;
}

UserProfile * UserProfileHandler::getUserProfile(const std::string & name) {
	UserProfile * result = new UserProfile();
	UserProfileFileStorage userProfileStorage(*result);
	userProfileStorage.profileLoadedFromBackupsEvent += profileLoadedFromBackupsEvent;
	userProfileStorage.profileCannotBeLoadedEvent += profileCannotBeLoadedEvent;
	if (!userProfileStorage.load(name)) {
		OWSAFE_DELETE(result);
	}

	return result;
}

bool UserProfileHandler::LoadUserProfileFromName(UserProfile * userprofile, const std::string & name) {
	UserProfileFileStorage userProfileStorage(*userprofile);
	if (userProfileStorage.load(name)) {
		return true;
	}	
	return false;
}

void UserProfileHandler::createAndSetUserProfile(SipAccount & sipAccount) {
	RecursiveMutex::ScopedLock lock(_mutex);

	UserProfile * userProfile = NULL;

	if (sipAccount.isEmpty()) {
		LOG_FATAL("cannot be empty");
	}

	std::string profileName = sipAccount.getFullIdentity();
	
	// when the sip account is attached to a userprofile it should be reconnected
	sipAccount.setConnected(false);
	////

	if (!userProfileExists(profileName)) {
		userProfile = new UserProfile();
		userProfile->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
		userProfile->wengoAccountValidityEvent +=
			boost::bind(&UserProfileHandler::wengoAccountValidityEventHandler, this, _1, _2);
		userProfile->setSipAccount(sipAccount);
	} else {
		LOG_ERROR("A UserProfile with the name: " + profileName + " already exists");
	}
}

bool UserProfileHandler::userProfileExists(const std::string & name) {
	bool result = false;

	if (!name.empty() && File::exists(UserProfileFileStorage::getProfilePath(name))) {
		result = true;
	}

	return result;
}

void UserProfileHandler::setCurrentUserProfile(const std::string & name, const SipAccount & sipAccount) {

	RecursiveMutex::ScopedLock lock(_mutex);

	UserProfile * result = getUserProfile(name);
	
	// Check if the desired UserProfile is different from the current UserProfile
	// and check if the WengoAccount of the current UserProfile is different from the given WengoAccount
	// (if so the WengoAccount (only 'password' and 'keep password' members) will be updated).
	if (!_currentUserProfile ||
		(_currentUserProfile &&
			((_currentUserProfile->getName() != name) ||
			((sipAccount.getUserPassword() != result->getSipAccount()->getUserPassword()) ||
				(sipAccount.isPasswordRemembered() != result->getSipAccount()->isPasswordRemembered()))))) {

		if (result) {
			// If the SipAccount is not empty, we update the one in UserProfile
			// This can happen if the password has been changed
			
			if (!sipAccount.isEmpty() ) {

				// to be virtual ?
				switch(sipAccount.getType()) {
					case SipAccount::SipAccountTypeBasic: {
					
						SipAccount * actSipAccount = result->getSipAccount();
						if (actSipAccount) {
						
							actSipAccount->setRegisterServerHostname(sipAccount.getRegisterServerHostname());
							actSipAccount->setRegisterServerPort(sipAccount.getRegisterServerPort());
							actSipAccount->setSIPProxyServerHostname(sipAccount.getSIPProxyServerHostname());
							actSipAccount->setSIPProxyServerPort(sipAccount.getSIPProxyServerPort());
							actSipAccount->setDisplayName(sipAccount.getDisplayName());
							actSipAccount->setIdentity(sipAccount.getIdentity());
							actSipAccount->setUsername(sipAccount.getUsername());
							actSipAccount->setPassword(sipAccount.getPassword());
							actSipAccount->setRealm(sipAccount.getRealm());
							actSipAccount->setVisibleName(sipAccount.getVisibleName());
							actSipAccount->enablePIM(sipAccount.isPIMEnabled());
						} else {
							SipAccount mySipAccount = sipAccount;
							result->setSipAccount(mySipAccount, false);
						}
					}
					break;
					case SipAccount::SipAccountTypeWengo: {
					
						const WengoAccount & refWengoAccount = dynamic_cast<const WengoAccount &>(sipAccount);
						WengoAccount * actWengoAccount = result->getWengoAccount();
						
						if (actWengoAccount) {
							actWengoAccount->setWengoLogin(refWengoAccount.getWengoLogin());
							actWengoAccount->setWengoPassword(refWengoAccount.getWengoPassword());
						} else {
							WengoAccount myWengoAccount = refWengoAccount;
							result->setSipAccount(myWengoAccount, false);
						}
					}
					break;
				}
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
			// log off
			if (_currentUserProfile && name.empty()) {

				_saveTimerRunning = false;
				_saveTimer.stop();

				_desiredUserProfile = NULL;
				currentUserProfileWillDieEvent(*this);
			// when name is empty or do not match any profile
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
		OWSAFE_DELETE(_currentUserProfile);

		uninitExtLibraries();
	}

	if (_desiredUserProfile) {
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
	_currentUserProfile->wengoAccountValidityEvent +=
		boost::bind(&UserProfileHandler::wengoAccountValidityEventHandlerForInitialize, this, _1, _2);
	_currentUserProfile->networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	initExtLibraries(File::convertPathSeparators(config.getConfigDir() + "profiles/") + _currentUserProfile->getName());
	_currentUserProfile->init();
}

void UserProfileHandler::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string profileName = config.getProfileLastUsedName();

	setCurrentUserProfile(profileName, SipAccount::empty);
}

void UserProfileHandler::saveUserProfile(UserProfile & userProfile) {
	if (_autoSave) {
		UserProfileFileStorage userProfileStorage(userProfile);
		userProfileStorage.save(userProfile.getName());
	}
}

void UserProfileHandler::profileChangedEventHandler() {
	RecursiveMutex::ScopedLock lock(_mutex);
	
	if (!_saveTimerRunning) {
		_saveTimerRunning = true;
		_saveTimer.start(5000, 5000, 1);
	}
}

void UserProfileHandler::saveTimerLastTimeoutEventHandler(Timer & sender) {
	{
		RecursiveMutex::ScopedLock lock(_mutex);
		if (!_saveTimerRunning) {
			LOG_WARN("Intercepted saveTimerLastTimeoutEventHandler");
			return;
		}
	}

	if (_currentUserProfile) {
		saveUserProfile(*_currentUserProfile);
	}

	{
		RecursiveMutex::ScopedLock lock(_mutex);
		_saveTimerRunning = false;
	}
}

void UserProfileHandler::wengoAccountValidityEventHandler(UserProfile & sender, bool valid) {
	typedef ThreadEvent2<void (UserProfile & sender, bool valid), UserProfile &, bool> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&UserProfileHandler::wengoAccountValidityEventHandlerThreadSafe, this, _1, _2),
			sender, valid);

	WengoPhone::getInstance().postEvent(event);
}

void UserProfileHandler::wengoAccountValidityEventHandlerThreadSafe(UserProfile & sender, bool valid) {
	if (valid) {
	
		saveUserProfile(sender);
		// We set the current UserProfile as currently only createAndSetUserProfile
		// can go here.
		setCurrentUserProfile(sender.getName(), *sender.getSipAccount());
	} else {
		sipAccountNotValidEvent(*this, *sender.getSipAccount());
	}

	UserProfile *senderPtr = &sender;
	OWSAFE_DELETE(senderPtr);
}

void UserProfileHandler::wengoAccountValidityEventHandlerForInitialize(UserProfile & sender, bool valid) {
	typedef ThreadEvent2<void (UserProfile & sender, bool valid), UserProfile &, bool> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&UserProfileHandler::wengoAccountValidityEventHandlerForInitializeThreadSafe, this, _1, _2),
			sender, valid);

	WengoPhone::getInstance().postEvent(event);
}

void UserProfileHandler::wengoAccountValidityEventHandlerForInitializeThreadSafe(UserProfile & sender, bool valid) {
	if (!valid) {
		SipAccount * sipaccount = _currentUserProfile->getSipAccount()->clone();
		OWSAFE_DELETE(_currentUserProfile);
		uninitExtLibraries();
		sipAccountNotValidEvent(*this, *sipaccount);
		OWSAFE_DELETE(sipaccount);
	} else {
		saveUserProfile(sender);

		if (_currentUserProfile->hasWengoAccount()) {
			WsUrl::setWengoAccount(_currentUserProfile->getWengoAccount());
		}

		userProfileInitializedEvent(*this, *_currentUserProfile);

		setLastUsedUserProfile(*_currentUserProfile);
	}
}

void UserProfileHandler::cleanupUserProfileDirectories() {

	StringList userProfiles = getUserProfileNames();
	StringList::iterator it;
	for (it = userProfiles.begin(); it != userProfiles.end(); it++) {

		if (String(*it).endsWith(".new") || String(*it).endsWith(".old")) {
			Config & config = ConfigManager::getInstance().getCurrentConfig();
			File profileDirectory(File::convertPathSeparators(config.getConfigDir() + "profiles/") + (*it));
			profileDirectory.remove();
		}
	}
}

void UserProfileHandler::initExtLibraries(const std::string & pathToProfile) {
	IMWrapperFactory::getFactory().init(pathToProfile);
}

void UserProfileHandler::uninitExtLibraries() {
	IMWrapperFactory::getFactory().terminate();
}
