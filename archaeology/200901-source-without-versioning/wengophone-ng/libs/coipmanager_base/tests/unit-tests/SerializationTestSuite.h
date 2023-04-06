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

#ifndef OWSERIALIZATIONTESTSUITE_H
#define OWSERIALIZATIONTESTSUITE_H

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/storage/AccountListFileStorage.h>

#include <util/Path.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_case;
using boost::unit_test_framework::test_suite;

class SerializationTest {
public:

	SerializationTest() {
	}

	void testAccountSerialization() {
		Account account1("login1", "password1", EnumAccountType::AccountTypeWengo);
		account1.setSavePassword(true);

		Account account2("login2", "password2", EnumAccountType::AccountTypeMSN);
		account2.setSavePassword(true);

		Account account3("login1", "password1", EnumAccountType::AccountTypeWengo);
		account3.setSavePassword(false);

		AccountList accountList;
		accountList.push_back(account1);
		accountList.push_back(account2);
		accountList.push_back(account3);

		const std::string ACCOUNTLIST_FILE = Path::getApplicationDirPath();
		AccountListFileStorage accountListFileStorage;
		accountListFileStorage.save(ACCOUNTLIST_FILE, accountList);

		
	}

	void testContactSerialization() {
	}

	void testContactGroupSerialization() {
	}

	void testUserProfileSerialization() {
	}

};

class SerializationTestSuite : public test_suite {
public:

	SerializationTestSuite() : test_suite("CoIpManager serialization test suite") {
		boost::shared_ptr<SerializationTest> coInstance(new SerializationTest());
		
		test_case * coAccountSerialization =
			BOOST_CLASS_TEST_CASE(&SerializationTest::testAccountSerialization, coInstance);
		test_case * coContactSerialization =
			BOOST_CLASS_TEST_CASE(&SerializationTest::testContactSerialization, coInstance);
		test_case * coContactGroupSerialization =
			BOOST_CLASS_TEST_CASE(&SerializationTest::testContactGroupSerialization, coInstance);
		test_case * coUserProfileSerialization =
			BOOST_CLASS_TEST_CASE(&SerializationTest::testUserProfileSerialization, coInstance);

		add(coAccountSerialization);
		add(coContactSerialization);
		add(coContactGroupSerialization);
		add(coUserProfileSerialization);
	}
};

#endif //OWSERIALIZATIONTESTSUITE_H
