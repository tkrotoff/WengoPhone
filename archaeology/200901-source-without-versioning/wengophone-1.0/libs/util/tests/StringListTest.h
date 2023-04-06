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

#ifndef STRINGLISTTEST_H
#define STRINGLISTTEST_H

#include <StringList.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class StringListTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(StringListTest);
	CPPUNIT_TEST(testOperatorIndex);
	CPPUNIT_TEST(testOperatorAdd);
	CPPUNIT_TEST(testSplit);
        CPPUNIT_TEST(testContains);
	CPPUNIT_TEST_SUITE_END();

public:

	void setUp() {
	}

	void tearDown() {
	}

	void testOperatorAdd() {
		testOperatorIndex();
	}

	void testOperatorIndex() {
		StringList strList;
		strList += "wengo0";
		strList += "wengo1";
		strList += "wengo2";
		strList += "wengo3";
		CPPUNIT_ASSERT(strList[0] == "wengo0");
		CPPUNIT_ASSERT(strList[1] == "wengo1");
		CPPUNIT_ASSERT(strList[2] == "wengo2");
		CPPUNIT_ASSERT(strList[3] == "wengo3");
		CPPUNIT_ASSERT(strList[4] == String::null);
		CPPUNIT_ASSERT(strList[4].empty());
	}

	void testSplit() {
		StringList tokens1 = StringList::split("four roses", "");
		CPPUNIT_ASSERT(tokens1[0] == "four roses");
		CPPUNIT_ASSERT(tokens1[0] != String::null);
		CPPUNIT_ASSERT(!tokens1[0].empty());

		StringList tokens2 = StringList::split("four roses");
		CPPUNIT_ASSERT(tokens2[0] == "four");
		CPPUNIT_ASSERT(tokens2[1] == "roses");
		CPPUNIT_ASSERT(tokens2[2] == String::null);
		CPPUNIT_ASSERT(tokens2[2].empty());
	}

        void testContains() {
                StringList strList;
                strList += "wengo";
                strList += "is";
                strList += "good";
                strList += "wengo";
                CPPUNIT_ASSERT(strList.contains("wengo") == 2);
                CPPUNIT_ASSERT(strList.contains("Wengo") == 0);
                CPPUNIT_ASSERT(strList.contains("Wengo", false) == 2);
        }
};

#endif	//STRINGLISTTEST_H
