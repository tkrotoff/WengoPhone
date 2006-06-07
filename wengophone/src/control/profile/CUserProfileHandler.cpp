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

#include "CUserProfileHandler.h"

#include <presentation/PFactory.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/profile/UserProfile.h>
#include <model/profile/UserProfileHandler.h>

#include <thread/Thread.h>

CUserProfileHandler::CUserProfileHandler(UserProfileHandler & userProfileHandler, 
	Thread & modelThread) 
: _userProfileHandler(userProfileHandler), _modelThread(modelThread) {
}

CUserProfileHandler::~CUserProfileHandler() {
}

std::vector<std::string> CUserProfileHandler::getUserProfileNames() {
	return _userProfileHandler.getUserProfileNames();
}

void CUserProfileHandler::createUserProfile(const WengoAccount & wengoAccount) {
	typedef ThreadEvent1<void (WengoAccount), WengoAccount> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&CUserProfileHandler::createUserProfileThreadSafe, this, _1), wengoAccount);

	_modelThread.postEvent(event);
}

void CUserProfileHandler::createUserProfileThreadSafe(WengoAccount wengoAccount) {
	_userProfileHandler.createUserProfile(wengoAccount);
}

void CUserProfileHandler::createAndSetUserProfile(const WengoAccount & wengoAccount) {
	typedef ThreadEvent1<void (WengoAccount), WengoAccount> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&CUserProfileHandler::createAndSetUserProfileThreadSafe, this, _1), wengoAccount);

	_modelThread.postEvent(event);
}

void CUserProfileHandler::createAndSetUserProfileThreadSafe(WengoAccount wengoAccount) {
	_userProfileHandler.createAndSetUserProfile(wengoAccount);
}

void CUserProfileHandler::setCurrentUserProfile(const std::string & login) {
	typedef ThreadEvent1<void (std::string), std::string> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&CUserProfileHandler::setUserProfileThreadSafe, this, _1), 
			login);

	_modelThread.postEvent(event);
}

void CUserProfileHandler::setUserProfileThreadSafe(std::string profileName) {
	_userProfileHandler.setCurrentUserProfile(profileName);
}

bool CUserProfileHandler::userProfileExists(const std::string & name) const {
	return _userProfileHandler.userProfileExists(name);
}

WengoAccount CUserProfileHandler::getWengoAccountOfUserProfile(const std::string & name) {
	UserProfile * userProfile = _userProfileHandler.getUserProfile(name);
	WengoAccount result("", "", false);

	if (userProfile && userProfile->hasWengoAccount()) {
		result = *userProfile->getWengoAccount();
	}

	if (userProfile) {
		delete userProfile;
	}

	return result;
}

void CUserProfileHandler::currentUserProfileReleased() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event =
		new MyThreadEvent(boost::bind(&CUserProfileHandler::currentUserProfileReleasedThreadSafe, this));

	_modelThread.postEvent(event);
}

void CUserProfileHandler::currentUserProfileReleasedThreadSafe() {
	PFactory::getFactory().reset();
	_userProfileHandler.currentUserProfileReleased();
}
