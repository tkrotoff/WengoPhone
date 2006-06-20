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

#ifndef OWUSERPROFILEHANDLER_H
#define OWUSERPROFILEHANDLER_H

#include <thread/Mutex.h>
#include <util/Event.h>
#include <util/Trackable.h>

#include <vector>
#include <string>

class Profile;
class Thread;
class UserProfile;
class WengoAccount;
class WengoPhone;

/**
 * Manages the UserProfiles.
 *
 * @author Philippe Bernery
 */
class UserProfileHandler : public Trackable {
public:

	enum UserProfileHandlerError {
		UserProfileHandlerErrorNoError,
		UserProfileHandlerErrorUserProfileAlreadyExists,
		UserProfileHandlerErrorWengoAccountNotValid
	};

	/**
	 * No current UserProfile has been set. This can happen at the first launch
	 * of the softphone when no UserProfile has been set.
	 *
	 * @param sender this class
	 */
	Event<void (UserProfileHandler & sender)> noCurrentUserProfileSetEvent;

	/**
	 * Emitted when a UserProfile will be destroyed.
	 *
	 * After the event, the old UserProfile reference will be invalidated.
	 *
	 * @param sender this class
	 */
	Event<void (UserProfileHandler & sender)> currentUserProfileWillDieEvent;

	/**
	 * A UserProfile has been set.
	 *
	 * @param sender this class
	 * @param userProfile the set UserProfile
	 */
	Event<void (UserProfileHandler & sender, 
		UserProfile & userProfile)> userProfileInitializedEvent;

	/**
	 * Emitted when the requested WengoAccount (given via setCurrentUserProfile) 
	 * is not valid.
	 *
	 * @param sender this class
	 * @param wengoAccount the WengoAccount provided by the user
	 */
	Event<void (UserProfileHandler & sender, 
		WengoAccount & wengoAccount)> wengoAccountNotValidEvent;

	/**
	 * Emitted when a non-Default UserProfile has been created and when a 
	 * Default UserProfile exists.
	 *
	 * Thus a dialog window can appear to propose to import ContactList and 
	 * IMAccounts from the Default UserProfile to the created UserProfile.
	 */
	Event<void (UserProfileHandler & sender, 
		const std::string & createdProfileName)> defaultUserProfileExistsEvent;

	UserProfileHandler(Thread & modelThread);

	~UserProfileHandler();

	/**
	 * Initializes the UserPofileHandler.
	 *
	 * During initialization, the UserProfileHandler will check for the last
	 * used UserProfile. If no UserProfile has been used, the 
	 * noCurrentUserProfileSetEvent will be emitted.
	 */
	void init();

	/**
	 * Gets the names of existing user profiles.
	 *
	 * These UserProfiles can then be loaded with getUserProfile.
	 *
	 * @return vector of UserProfiles
	 */
	std::vector<std::string> getUserProfileNames();

	/**
	 * Sets the current UserProfile.
	 *
	 * A non empty WengoAccount can be given in parameter. It will be used
	 * to update password and autologin attributes of the UserProfile.
	 *
	 * @param name the name of the current UserProfile to set
	 * @param wengoAccount the WengoAccount that will update the UserProfile
	 */
	void setCurrentUserProfile(const std::string & name,
		const WengoAccount & wengoAccount);

	/**
	 * Creates a new UserProfile.
	 *
	 * Calling this function will set the current UserProfile as the
	 * the created UserProfile.
	 *
	 * @param wengoAccount a UserProfile is identified with a WengoAccount.
	 * If the WengoAccount is empty (that is it has an empty login), a 'Default'
	 * UserProfile is created.
	 *
	 * @return an error code
	 */
	UserProfileHandlerError createUserProfile(const WengoAccount & wengoAccount);

	/**
	 * Creates a new UserProfile and set it as current UserProfile.
	 *
	 * @param wengoAccount a UserProfile is identified with a WengoAccount.
	 * If the WengoAccount is empty (that is to say it has an empty login),
	 *  a 'Default' UserProfile is created.
	 */
	void createAndSetUserProfile(const WengoAccount & wengoAccount);

	/**
	 * Check if a UserProfile exists.
	 *
	 * @param name name of the UserProfile to check
	 * @return true if a UserProfile with the given name exists
	 */
	bool userProfileExists(const std::string & name);

	/**
	 * Must be called when the current UserProfile is released and can be
	 * destroyed.
	 */
	void currentUserProfileReleased();

	/**
	 * Imports contacts and IM accounts from the 'Default' profile (if it
	 * exists) to the given profile and delete the 'Default' profile.
	 */
	void importDefaultProfileToProfile(const std::string & profileName);

	/**
	 * Saves the last used UserProfile in Config.
	 */
	void save();

	/**
	 * Gets a UserProfile.
	 *
	 * Used by model and control. Must not be used by presentation
	 *
	 * @param name the name of the desired UserProfile
	 * @return the UserProfile or NULL if no UserProfile with the given found
	 */
	UserProfile * getUserProfile(const std::string & name);

private:

	/**
	 * Initializes the current UserProfile.
	 */
	void initializeCurrentUserProfile();

	/*
	 * @see Profile::profileChangedEvent
	 */
	void profileChangedEventHandler(Profile & sender);

	/**
	 * Sets the last used UserProfile.
	 *
	 * @param the last used UserProfile
	 */
	void setLastUsedUserProfile(const UserProfile & userProfile);

	/**
	 * Saves the UserProfile.
	 *
	 * @param userProfile the UserProfile to save
	 */
	void saveUserProfile(UserProfile & userProfile);

	/**
	 * @see importDefaultProfileToProfile
	 * When calling importDefaultProfileToProfile, the current UserProfile
	 * will be destroyed and thus UserProfileHandler must wait for the 
	 * release of the currentUserProfile. When currentUserProfileReleased
	 * will be called it will call actuallyImportDefaultProfileToProfile to
	 * actually import the default profile to the desired UserProfile.
	 */
	void actuallyImportDefaultProfileToProfile();

	/**
	 * True if UserProfileHandler must call actuallyImportDefaultProfileToProfile
	 * in currentUserProfileReleased.
	 */
	bool _importDefaultProfileToProfile;

	/** 
	 * Name of the UserProfile to import when calling
	 * actuallyImportDefaultProfileToProfile.
	 */
	std::string _nameOfProfileToImport;	

	Mutex _mutex;

	UserProfile * _currentUserProfile;

	UserProfile * _desiredUserProfile;

	Thread & _modelThread;
};

#endif //OWUSERPROFILEHANDLER_H
