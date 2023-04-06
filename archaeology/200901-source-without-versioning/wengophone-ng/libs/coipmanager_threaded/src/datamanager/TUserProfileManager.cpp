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

#include <coipmanager_threaded/datamanager/TUserProfileManager.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeConnect.h>
#include <util/PostEvent.h>

TUserProfileManager::TUserProfileManager(TCoIpManager & tCoIpManager)
	: _tCoIpManager(tCoIpManager),
	_tAccountManager(tCoIpManager),
	_tContactManager(tCoIpManager) {

	SAFE_CONNECT(getUserProfileManager(), SIGNAL(userProfileSetSignal(UserProfile)),
		SIGNAL(userProfileSetSignal(UserProfile)));
	SAFE_CONNECT(getUserProfileManager(), SIGNAL(userProfileUpdatedSignal(UserProfile)),
		SIGNAL(userProfileUpdatedSignal(UserProfile)));
	SAFE_CONNECT(getUserProfileManager(), SIGNAL(userProfileLoggedOffSignal(UserProfile)),
		SIGNAL(userProfileLoggedOffSignal(UserProfile)));
}

TUserProfileManager::~TUserProfileManager() {
}

TAccountManager & TUserProfileManager::getTAccountManager() {
	return _tAccountManager;
}

TContactManager & TUserProfileManager::getTContactManager() {
	return _tContactManager;
}

UserProfile TUserProfileManager::getCopyOfUserProfile() {
	return getUserProfileManager()->getCopyOfUserProfile();
}

UserProfileManager * TUserProfileManager::getUserProfileManager() {
	return &_tCoIpManager.getCoIpManager().getUserProfileManager();
}

void TUserProfileManager::setUserProfile(const UserProfile & userProfile) {
	PostEvent::invokeMethod(getUserProfileManager(), "setUserProfile",
		Q_ARG(UserProfile, userProfile));
}

void TUserProfileManager::updateUserProfile(const UserProfile & userProfile) {
	PostEvent::invokeMethod(getUserProfileManager(), "updateUserProfile",
		Q_ARG(UserProfile, userProfile));
}

void TUserProfileManager::logOff() {
	PostEvent::invokeMethod(getUserProfileManager(), "logOff");
}
