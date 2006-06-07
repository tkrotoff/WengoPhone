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

#ifndef CUSERPROFILEHANDLER_H
#define CUSERPROFILEHANDLER_H

#include <model/account/wengo/WengoAccount.h>

#include <string>

class UserProfileHandler;
class Thread;

/**
 * Control of UserProfileHandler.
 *
 * @author Philippe Bernery
 */
class CUserProfileHandler {
public:

	CUserProfileHandler(UserProfileHandler & userProfileHandler,
		Thread & modelThread);

	~CUserProfileHandler();

	/**
	 * @see UserProfileHandler::getUserProfileNames
	 */
	std::vector<std::string> getUserProfileNames();

	/**
	 * @return true if the UserProfile named 'name' exists
	 */
	bool userProfileExists(const std::string & name) const;

	/**
	 * @see UserProfileHandler::createUserProfile
	 */
	void createUserProfile(const WengoAccount & wengoAccount);

	/**
	 * @see UserProfileHandler::createAndSetUserProfile
	 */
	void createAndSetUserProfile(const WengoAccount & wengoAccount);

	/**
	 * @see UserProfileHandler::setUserProfile
	 */
	void setCurrentUserProfile(const std::string & profileName);

	/**
	 * @see UserProfileHandler::currentUserProfileReleased
	 */
	void currentUserProfileReleased();

	/**
	 * Return a copy of the WengoAccount of the UserProfile named 'name'.
	 *
	 * If no UserProfile found the WengoAccount.getWengoLogin() will return
	 * an empty string.
	 *
	 * @param name the UserProfile name
	 * @return the WengoAccount of the UserProfile named 'name'
	 */
	WengoAccount getWengoAccountOfUserProfile(const std::string & name);

private:

	/**
	 * @see createUserProfile
	 */
	void createUserProfileThreadSafe(WengoAccount wengoAccount);

	/**
	 * @see createAndSetUserProfile
	 */
	void createAndSetUserProfileThreadSafe(WengoAccount wengoAccount);

	/**
	 * @see setCurrentUserProfile
	 */
	void setUserProfileThreadSafe(std::string profileName);

	/**
	 * @see currentUserProfileReleased
	 */
	void currentUserProfileReleasedThreadSafe();

	UserProfileHandler & _userProfileHandler;

	Thread & _modelThread;
};

#endif //CUSERPROFILEHANDLER_H
