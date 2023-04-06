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

#ifndef STARTUPTEST_H
#define STARTUPTEST_H

#include <Startup.h>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class StartupTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(StartupTest);
	CPPUNIT_TEST(testSetStartup);
	CPPUNIT_TEST(testIsStartup);
	CPPUNIT_TEST_SUITE_END();
public:

	void setUp() {
        	_startup = new Startup("C:/toto/toto.exe", "toto");
	}

	void tearDown() {
                delete _startup;
	}

	void testSetStartup() {
		CPPUNIT_ASSERT(_startup->setStartup());
        }

        void testIsStartup() {
		CPPUNIT_ASSERT(_startup->isStartup());
		CPPUNIT_ASSERT(_startup->setStartup(false));
                CPPUNIT_ASSERT(!_startup->isStartup());
        }

private:

        Startup * _startup;
};

#endif	//STARTUPTEST_H
