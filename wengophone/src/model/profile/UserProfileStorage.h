/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef USERPROFILESTORAGE_H
#define USERPROFILESTORAGE_H

#include <serialization/Storage.h>

class UserProfile;

/**
 * UserProfile storage.
 *
 * @ingroup model
 * @author Philippe Bernery
 */
class UserProfileStorage : public Storage {
public:

	UserProfileStorage(UserProfile & userProfile)
		: _userProfile(userProfile) { }

	virtual ~UserProfileStorage() { }

	/**
	 * load a user profile.
	 *
	 * @param url url to user profile. This url must point to a directory
	 * @return true if ok
	 */
	virtual bool load(const std::string & url) = 0;

	/**
	 * save a user profile.
	 *
	 * @param url url to user profile. This url must point to a directory
	 * @return true if ok
	 */
	virtual bool save(const std::string & url) = 0;

protected:

	UserProfile & _userProfile;
};

#endif //USERPROFILESTORAGE_H
