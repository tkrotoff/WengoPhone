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

#ifndef OWDATAMANAGERTESTSUITE_H
#define OWDATAMANAGERTESTSUITE_H

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeDelete.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_case;
using boost::unit_test_framework::test_suite;

class UserProfileManagerTest {
public:

	UserProfileManagerTest() {
		_coIpManager = NULL;
	}

	void testEmptyUserProfile() {
		// UserProfile id should be an empty string just
		// after CoIpManager initialization.
		initialize();

		UserProfile userProfile = _coIpManager->getUserProfileManager().getCopyOfUserProfile();
		BOOST_CHECK(userProfile.getName().empty());

		uninitialize();
	}

	void testSetUserProfile() {
		initialize();

		UserProfile userProfile1;
		userProfile1.setName("profile1");
		_coIpManager->getUserProfileManager().setUserProfile(userProfile1);
		BOOST_CHECK(userProfile1.getName() == _coIpManager->getUserProfileManager().getCopyOfUserProfile().getName());

		UserProfile userProfile2;
		userProfile2.setName("profile2");
		_coIpManager->getUserProfileManager().setUserProfile(userProfile2);
		BOOST_CHECK(userProfile2.getName() == _coIpManager->getUserProfileManager().getCopyOfUserProfile().getName());

		uninitialize();
	}

	void testUpdateUserProfile() {
		initialize();

		// Setting a UserProfile
		UserProfile userProfile;
		userProfile.setFirstName("test-profile");
		_coIpManager->getUserProfileManager().setUserProfile(userProfile);
		BOOST_CHECK(userProfile.getFirstName() == _coIpManager->getUserProfileManager().getCopyOfUserProfile().getFirstName());
		////

		// Setting the name on UserProfile is not enough
		userProfile.setFirstName("test-profile2");
		BOOST_CHECK(userProfile.getFirstName() != _coIpManager->getUserProfileManager().getCopyOfUserProfile().getFirstName());
		////

		// Trying to update UserProfile with another one (different UUID)
		UserProfile userProfile2;
		userProfile2.setFirstName("test-profile3");
		BOOST_CHECK(!_coIpManager->getUserProfileManager().updateUserProfile(userProfile2));
		BOOST_CHECK(userProfile2.getFirstName() != _coIpManager->getUserProfileManager().getCopyOfUserProfile().getFirstName());			////

		// Updating UserProfile with the right one
		BOOST_CHECK(_coIpManager->getUserProfileManager().updateUserProfile(userProfile));
		BOOST_CHECK(userProfile.getFirstName() == _coIpManager->getUserProfileManager().getCopyOfUserProfile().getFirstName());
		////

		uninitialize();
	}

	void testContactGroup() {
		initialize();

		// Setting UserProfile
		UserProfile userProfile;
		_coIpManager->getUserProfileManager().setUserProfile(userProfile);
		////

		// Adding Contacts
		Contact contact1, contact2;
		_coIpManager->getUserProfileManager().getContactManager().add(contact1);
		_coIpManager->getUserProfileManager().getContactManager().add(contact2);
		////

		// Setting groups
		static const std::string GROUP1 = "test-1";
		static const std::string GROUP2 = "test-2";

		BOOST_CHECK(_coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactGroupList().empty());

		// should return false as the group has not been added yet
		BOOST_CHECK(!_coIpManager->getUserProfileManager().getContactGroupManager().addToGroup(GROUP1, contact1.getUUID()));

		std::string group1id = _coIpManager->getUserProfileManager().getContactGroupManager().add(GROUP1);
		BOOST_CHECK(!group1id.empty());
		BOOST_CHECK(_coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactGroupList().size() == 1);

		// should return group1id as the group is already added
		BOOST_CHECK(group1id == _coIpManager->getUserProfileManager().getContactGroupManager().add(GROUP1));
		BOOST_CHECK(_coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactGroupList().size() == 1);

		// should return true as the contact is not yet in the group
		BOOST_CHECK(_coIpManager->getUserProfileManager().getContactGroupManager().addToGroup(group1id, contact1.getUUID()));
		ContactList contactList = _coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactList();
		Contact *realContact1 = ContactListHelper::getCopyOfContact(contactList, contact1.getUUID());
		BOOST_CHECK(realContact1->getGroupList().size() == 1);
		BOOST_CHECK((*realContact1->getGroupList().begin()) == group1id);
		OWSAFE_DELETE(realContact1);

		// should return false as the contact is already in the group
		BOOST_CHECK(!_coIpManager->getUserProfileManager().getContactGroupManager().addToGroup(group1id, contact1.getUUID()));
		contactList = _coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactList();
		realContact1 = ContactListHelper::getCopyOfContact(contactList, contact1.getUUID());
		BOOST_CHECK(realContact1->getGroupList().size() == 1);
		BOOST_CHECK((*realContact1->getGroupList().begin()) == group1id);
		OWSAFE_DELETE(realContact1);

		std::string group2id = _coIpManager->getUserProfileManager().getContactGroupManager().add(GROUP2);
		BOOST_CHECK(!group2id.empty());
		BOOST_CHECK(_coIpManager->getUserProfileManager().getCopyOfUserProfile().getContactGroupList().size() == 2);

		uninitialize();
	}

private:

	void initialize() {
		CoIpManagerConfig config;
		config.set(CoIpManagerConfig::AUTO_SAVE_KEY, false);
		_coIpManager = new CoIpManager(config);
		_coIpManager->initialize();
	}

	void uninitialize() {
		OWSAFE_DELETE(_coIpManager);
	}

	CoIpManager *_coIpManager;

};

class DataManagerTestSuite : public test_suite {
public:

	DataManagerTestSuite() : test_suite("DataManager test suite") {
		boost::shared_ptr<UserProfileManagerTest> upInstance(new UserProfileManagerTest());

		test_case * upTestEmpty = BOOST_CLASS_TEST_CASE(&UserProfileManagerTest::testEmptyUserProfile, upInstance);
		test_case * upTestSet = BOOST_CLASS_TEST_CASE(&UserProfileManagerTest::testSetUserProfile, upInstance);
		test_case * upTestUpdate = BOOST_CLASS_TEST_CASE(&UserProfileManagerTest::testUpdateUserProfile, upInstance);
		test_case * upTestContactGroup = BOOST_CLASS_TEST_CASE(&UserProfileManagerTest::testContactGroup, upInstance);

		add(upTestEmpty);
		add(upTestSet);
		add(upTestUpdate);
		add(upTestContactGroup);
	}
};

#endif	//OWDATAMANAGERTESTSUITE_H
