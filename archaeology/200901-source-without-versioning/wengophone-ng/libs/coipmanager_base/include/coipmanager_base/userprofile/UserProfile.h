/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
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

#ifndef OWUSERPROFILE_H
#define OWUSERPROFILE_H

#include <coipmanager_base/profile/Profile.h>

#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/contact/ContactGroupList.h>

/**
 * Handle the profile of a User.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API UserProfile : public Profile {
	friend class UserProfileFileStorage;
	friend class UserProfileManager;
public:

	UserProfile();

	UserProfile(const UserProfile & userProfile);

	UserProfile & operator=(const UserProfile & userProfile);

	~UserProfile();

	AccountList & getAccountList();

	ContactList & getContactList();

	ContactGroupList & getContactGroupList();

	void setName(const std::string & name);
	const std::string & getName() const;

	/**
	 * Returns true if the UserProfile is an empty UserProfile.
	 * An empty UserProfile has an empty name.
	 */
	bool isEmpty() const;

private:

	void copy(const UserProfile & userProfile);

	AccountList _accountList;

	ContactList _contactList;

	ContactGroupList _contactGroupList;

	std::string _profileName;
};

#endif	//OWUSERPROFILE_H
