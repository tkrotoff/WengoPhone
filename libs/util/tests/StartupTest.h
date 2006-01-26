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

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

class StartupTest {
public:

	StartupTest() {
		_startup = new Startup("C:/toto/toto.exe", "toto");
	}

	~StartupTest() {
		delete _startup;
	}

	void testSetStartup() {
		BOOST_CHECK(_startup->setStartup());
	}

	void testIsStartup() {
		BOOST_CHECK(_startup->isStartup());
		BOOST_CHECK(_startup->setStartup(false));
		BOOST_CHECK(!_startup->isStartup());
	}

private:

	Startup * _startup;
};

class StartupTestSuite : public test_suite {
public:

	StartupTestSuite() : test_suite("StartupTestSuite") {
		boost::shared_ptr<StartupTest> instance(new StartupTest());

		test_case * testSetStartup = BOOST_CLASS_TEST_CASE(&StartupTest::testSetStartup, instance);
		test_case * testIsStartup = BOOST_CLASS_TEST_CASE(&StartupTest::testIsStartup, instance);

		add(testSetStartup);
		add(testIsStartup);
	}
};

#endif	//STARTUPTEST_H
