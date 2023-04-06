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

#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager/CoIpManager.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>

UserProfileManager::UserProfileManager(CoIpManager & coIpManager)
	: _coIpManager(coIpManager),
	_accountManager(_userProfile._accountList),
	_contactManager(_userProfile._contactList),
	_contactGroupManager(*this, _userProfile._contactGroupList) {

	_mutex = new QMutex(QMutex::Recursive);
	_userProfile.setName(String::null);
}

UserProfileManager::~UserProfileManager() {
	OWSAFE_DELETE(_mutex);
}

AccountManager & UserProfileManager::getAccountManager() {
	QMutexLocker lock(_mutex);

	return _accountManager;
}

ContactManager & UserProfileManager::getContactManager() {
	QMutexLocker lock(_mutex);

	return _contactManager;
}

ContactGroupManager & UserProfileManager::getContactGroupManager() {
	QMutexLocker lock(_mutex);

	return _contactGroupManager;
}

UserProfile UserProfileManager::getCopyOfUserProfile() {
	QMutexLocker lock(_mutex);

	return _userProfile;
}

void UserProfileManager::setUserProfile(const UserProfile & userProfile) {
	QMutexLocker lock(_mutex);

	_coIpManager.releaseCoIpManager();

	_userProfile = userProfile;

	_coIpManager.initializeCoIpManager();

	userProfileSetSignal(userProfile);
}

bool UserProfileManager::updateUserProfile(const UserProfile & userProfile) {
	QMutexLocker lock(_mutex);

	bool result = false;

	if (userProfile.getUUID() == _userProfile.getUUID()) {
		_userProfile = userProfile;
		userProfileUpdatedSignal(userProfile);
		result = true;
	} else {
		LOG_ERROR("the given UserProfile has not the same UUID as the one set");
	}

	return result;
}

void UserProfileManager::logOff() {
	QMutexLocker lock(_mutex);

	_coIpManager.releaseCoIpManager();

	UserProfile userProfile;
	userProfile.setName(String::null);

	UserProfile oldUserProfile = _userProfile;

	_userProfile = userProfile;

	userProfileLoggedOffSignal(oldUserProfile);
}
