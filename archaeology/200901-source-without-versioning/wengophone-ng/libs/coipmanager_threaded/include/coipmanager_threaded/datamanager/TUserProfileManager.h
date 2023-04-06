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

#ifndef OWTUSERPROFILEMANAGER_H
#define OWTUSERPROFILEMANAGER_H

#include <coipmanager_threaded/datamanager/TAccountManager.h>
#include <coipmanager_threaded/datamanager/TContactManager.h>

#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/StringList.h>

class UserProfileManager;

/**
 * Threaded UserProfile management.
 *
 * @see UserProfileManager
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_THREADED_API TUserProfileManager : public QObject {
	Q_OBJECT
public:

	/**
	 * see UserProfileManager::UserProfileManager()
	 */
	TUserProfileManager(TCoIpManager & tCoIpManager);

	/**
	 * see UserProfileManager::~UserProfileManager()
	 */
	~TUserProfileManager();

	/**
	 * see UserProfileManager::getAccountManager()
	 */
	TAccountManager & getTAccountManager();

	/**
	 * see UserProfileManager::getContactManager()
	 */
	TContactManager & getTContactManager();

	/**
	 * see UserProfileManager::getUserProfile()
	 */
	UserProfile getCopyOfUserProfile();

	/**
	 * @see UserProfileManager::setUserProfile()
	 */
	void setUserProfile(const UserProfile & userProfile);

	/**
	 * @see UserProfileManager::updateUserProfile()
	 */
	void updateUserProfile(const UserProfile & userProfile);

	/**
	 * @see UserProfileManager::logOff()
	 */
	void logOff();

Q_SIGNALS:

	/**
	 * @see UserProfileManager::userProfileSetSignal
	 */
	void userProfileSetSignal(UserProfile userProfile);

	/**
	 * @see UserProfileManager::userProfileUpdatedSignal
	 */
	void userProfileUpdatedSignal(UserProfile userProfile);

	/**
	 * @see UserProfileManager::userProfileLoggedOffSignal
	 */
	void userProfileLoggedOffSignal(UserProfile userProfile);

private:

	/**
	 * Code factorization.
	 */
	UserProfileManager * getUserProfileManager();

	TCoIpManager & _tCoIpManager;

	TAccountManager _tAccountManager;

	TContactManager _tContactManager;
};

#endif	//OWTUSERPROFILEMANAGER_H
