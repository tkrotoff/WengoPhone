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

#ifndef OWUSERPROFILEMANAGER_H
#define OWUSERPROFILEMANAGER_H

#include <coipmanager_base/userprofile/UserProfile.h>

#include <coipmanager/datamanager/AccountManager.h>
#include <coipmanager/datamanager/ContactManager.h>
#include <coipmanager/datamanager/ContactGroupManager.h>

#include <QtCore/QMutex>
#include <QtCore/QObject>

class CoIpManager;

/**
 * Manages the UserProfiles.
 *
 * There is always a UserProfile set, but it can be empty.
 * TODO: a state could be added to know if the UserProfile has been set or not
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API UserProfileManager : public QObject {
	Q_OBJECT
public:

	UserProfileManager(CoIpManager & coIpManager);

	~UserProfileManager();

	AccountManager & getAccountManager();

	ContactManager & getContactManager();

	ContactGroupManager & getContactGroupManager();

	/**
	 * Gets the current UserProfile.
	 *
	 * @return a copy of the current UserProfile
	 */
	UserProfile getCopyOfUserProfile();

Q_SIGNALS:

	/**
	 * Emitted when a UserProfile has been set.
	 */
	void userProfileSetSignal(UserProfile userProfile);

	/**
	 * Emitted when the UserProfile has been updated.
	 */
	void userProfileUpdatedSignal(UserProfile userProfile);

	/**
	 * Emitted when the UserProfile has been logged off.
	 */
	void userProfileLoggedOffSignal(UserProfile userProfile);

public Q_SLOTS:

	/**
	 * Sets the UserProfile to use.
	 *
	 * Thread-safe.
	 *
	 * Warning!!!
	 * After calling setUserProfile() you must wait for userProfileSetEvent before to do anything else
	 * otherwise CoIpManager will crash
	 */
	void setUserProfile(const UserProfile & userProfile);

	/**
	 * Updates the current UserProfile.
	 *
	 * Thread-safe.
	 *
	 * @return true if actually updated (is currently set)
	 */
	bool updateUserProfile(const UserProfile & userProfile);

	/**
	 * Free everything and set an empty UserProfile.
	 *
	 * Thread-safe.
	 */
	void logOff();

private:

	CoIpManager & _coIpManager;

	UserProfile _userProfile;

	AccountManager _accountManager;

	ContactManager _contactManager;

	ContactGroupManager _contactGroupManager;

	QMutex * _mutex;
};

#endif	//OWUSERPROFILEMANAGER_H
