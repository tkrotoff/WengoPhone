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

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class StringTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(StringTest);
	CPPUNIT_TEST(testIsEmpty);
	CPPUNIT_TEST(testToLowerCase);
	CPPUNIT_TEST(testToUpperCase);
	CPPUNIT_TEST(testToInteger);
	CPPUNIT_TEST(testFromNumber);
	CPPUNIT_TEST(testReplace);
	CPPUNIT_TEST_SUITE_END();

public:

	void setUp() {
		_lowerCaseString = "**** wengo is cool!!! ####";
		_upperCaseString = "**** WENGO IS COOL!!! ####";
	}

	void tearDown() {
	}

	void testIsEmpty() {
		String str1("");
		CPPUNIT_ASSERT(str1.empty());

		String str2 = String::null;
		CPPUNIT_ASSERT(str2.empty());
	}

	void testToLowerCase() {
		CPPUNIT_ASSERT(_upperCaseString.toLowerCase() == _lowerCaseString);
	}

	void testToUpperCase() {
		CPPUNIT_ASSERT(_lowerCaseString.toUpperCase() == _upperCaseString);
	}

	void testToInteger() {
		String str1("284910");
		int i1 = 284910;
		CPPUNIT_ASSERT(str1.toInteger() == i1);

		String str2("-284910");
		int i2 = -284910;
		CPPUNIT_ASSERT(str2.toInteger() == i2);
	}

	void testFromNumber() {
		String str1("284910");
		CPPUNIT_ASSERT(str1 == String::getFromNumber(str1.toInteger()));

		String str2("-284910");
		CPPUNIT_ASSERT(str2 == String::getFromNumber(str2.toInteger()));
	}

	void testReplace() {
		String str1("Wengo is not cool!");
		String str2("Wengo is cool!");
		str1.replace("not ", String::null);
		CPPUNIT_ASSERT(str1 == str2);

		str1.replace("cool", "not cool");
		CPPUNIT_ASSERT(str1 != str2);
		str2.replace("cool", "not cool");
		CPPUNIT_ASSERT(str1 == str2);

		String str3("Wengo*Wengo/Wengo:Wengo");
		String str4("cool*cool/cool:cool");
		str3.replace("Wengo", "cool");
		CPPUNIT_ASSERT(str3 == str4);
	}

private:

	String _lowerCaseString;

	String _upperCaseString;
};

#endif	//STRINGTEST_H
