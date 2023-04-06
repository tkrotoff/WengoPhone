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

#ifndef OWCONTACTTESTSUITE_H
#define OWCONTACTTESTSUITE_H

#include <coipmanager_base/contact/Contact.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_case;
using boost::unit_test_framework::test_suite;

class ContactTest {
public:

	ContactTest() {
	}

	void testAddRemoveIMContact() {
		Contact contact;
		IMContact imContact1, imContact2;

		// Contact::IMContactList is empty
		BOOST_CHECK(contact.getIMContactList().size() == 0);
		////
		
		// Contact::IMContactList has an IMContact
		contact.addIMContact(imContact1);
		BOOST_CHECK(contact.getIMContactList().size() == 1);
		BOOST_CHECK(imContact1 == *contact.getIMContactList().begin());
		/////

		// Nothing changes because imContact1 already added
		contact.addIMContact(imContact1);
		BOOST_CHECK(contact.getIMContactList().size() == 1);
		BOOST_CHECK(imContact1 == *contact.getIMContactList().begin());
		/////

		// Contact has two IMContacts.
		contact.addIMContact(imContact2);
		BOOST_CHECK(contact.getIMContactList().size() == 2);
		////

		// Contact has one IMContact
		contact.removeIMContact(imContact1.getUUID());
		BOOST_CHECK(contact.getIMContactList().size() == 1);
		BOOST_CHECK(!(imContact1 == *contact.getIMContactList().begin()));
		BOOST_CHECK(imContact2 == *contact.getIMContactList().begin());
		////

		// Contact is empty
		contact.removeIMContact(imContact2.getUUID());
		BOOST_CHECK(contact.getIMContactList().size() == 0);
		////
	}

	void testContactGroup() {
		Contact contact;
		IMContact imContact;

		// Group lists empty
		BOOST_CHECK(contact.getGroupList().size() == 0);
		BOOST_CHECK(imContact.getGroupList().size() == 0);
		////

		// Checking if group actually to IMContact
		static const std::string GROUP1 = "test-1";
		imContact.addToGroup(GROUP1);
		BOOST_CHECK(imContact.getGroupList().size() == 1);
		BOOST_CHECK((*imContact.getGroupList().begin()) == GROUP1);
		////

		// Adding an IMContact to a Contact should also synchronize
		// the group list of Contact. Here the group list of the IMContact and
		// the Contact should be the same.
		contact.addIMContact(imContact);
		BOOST_CHECK(contact.getGroupList().size() == 1);
		BOOST_CHECK((*contact.getGroupList().begin()) == (*imContact.getGroupList().begin()));
		////

		// Checking the add of a second group
		static const std::string GROUP2 = "test-2";
		imContact.addToGroup(GROUP2);
		BOOST_CHECK(imContact.getGroupList().size() == 2);
		BOOST_CHECK(contact.getGroupList().size() == 1);
		////

		// Updating the Contact with the updated IMContact
		contact.updateIMContact(imContact);
		BOOST_CHECK(contact.getGroupList().size() == 2);
		BOOST_CHECK(contact.getGroupList() == imContact.getGroupList());
		////

		// Adding a Contact to a group should not change groups of inserted
		// IMContacts.
		static const std::string GROUP3 = "test-3";
		BOOST_CHECK(contact.addToGroup(GROUP3));
		BOOST_CHECK(contact.getGroupList().size() == 3);
		IMContact insertedIMContact = *contact.getIMContactList().begin();
		BOOST_CHECK(insertedIMContact.getGroupList().size() == 2);
		////
	}

};

class ContactTestSuite : public test_suite {
public:

	ContactTestSuite() : test_suite("CoIpManager Contact test suite") {
		boost::shared_ptr<ContactTest> coInstance(new ContactTest());
		
		test_case * coAddRemoveIMContact =
			BOOST_CLASS_TEST_CASE(&ContactTest::testAddRemoveIMContact, coInstance);
		test_case * coContactGroup =
			BOOST_CLASS_TEST_CASE(&ContactTest::testContactGroup, coInstance);

		add(coAddRemoveIMContact);
		add(coContactGroup);
	}
};

#endif //OWCONTACTTESTSUITE_H
