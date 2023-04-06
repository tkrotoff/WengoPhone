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

#ifndef STRINGTEST_H
#define STRINGTEST_H

#include <StringList.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

class StringTest {
public:

	StringTest() {
		_lowerCaseString = "**** wengo is cool!!! ####";
		_upperCaseString = "**** WENGO IS COOL!!! ####";
	}

	void testIsEmpty() {
		String str1("");
		BOOST_CHECK(str1.empty());

		String str2 = String::null;
		BOOST_CHECK(str2.empty());
	}

	void testToLowerCase() {
		BOOST_CHECK(_upperCaseString.toLowerCase() == _lowerCaseString);
	}

	void testToUpperCase() {
		BOOST_CHECK(_lowerCaseString.toUpperCase() == _upperCaseString);
	}

	void testToInteger() {
		String str1("284910");
		int i1 = 284910;
		BOOST_CHECK(str1.toInteger() == i1);

		String str2("-284910");
		int i2 = -284910;
		BOOST_CHECK(str2.toInteger() == i2);
	}

	void testFromNumber() {
		String str1("284910");
		BOOST_CHECK(str1 == String::fromNumber(str1.toInteger()));

		String str2("-284910");
		BOOST_CHECK(str2 == String::fromNumber(str2.toInteger()));
	}

	void testReplace() {
		String str1("Wengo is not cool!");
		String str2("Wengo is cool!");
		str1.replace("not ", String::null);
		BOOST_CHECK(str1 == str2);

		str1.replace("cool", "not cool");
		BOOST_CHECK(str1 != str2);
		str2.replace("cool", "not cool");
		BOOST_CHECK(str1 == str2);

		String str3("Wengo*Wengo/Wengo:Wengo");
		String str4("cool*cool/cool:cool");
		str3.replace("Wengo", "cool");
		BOOST_CHECK(str3 == str4);
	}

private:

	String _lowerCaseString;

	String _upperCaseString;
};

class StringTestSuite : public test_suite {
public:

	StringTestSuite() : test_suite("StringTestSuite") {
		boost::shared_ptr<StringTest> instance(new StringTest());

		test_case * testIsEmpty = BOOST_CLASS_TEST_CASE(&StringTest::testIsEmpty, instance);
		test_case * testToLowerCase = BOOST_CLASS_TEST_CASE(&StringTest::testToLowerCase, instance);
		test_case * testToUpperCase = BOOST_CLASS_TEST_CASE(&StringTest::testToUpperCase, instance);
		test_case * testToInteger = BOOST_CLASS_TEST_CASE(&StringTest::testToInteger, instance);
		test_case * testFromNumber = BOOST_CLASS_TEST_CASE(&StringTest::testFromNumber, instance);
		test_case * testReplace = BOOST_CLASS_TEST_CASE(&StringTest::testReplace, instance);

		add(testIsEmpty);
		add(testToLowerCase);
		add(testToUpperCase);
		add(testToInteger);
		add(testFromNumber);
		add(testReplace);
	}
};

#endif	//STRINGTEST_H
