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

#ifndef OWCOIPMANAGERTESTSUITE_H
#define OWCOIPMANAGERTESTSUITE_H

#include <coipmanager/CoIpManager.h>

#include <util/SafeDelete.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_case;
using boost::unit_test_framework::test_suite;

class CoIpManagerTest {
public:

	CoIpManagerTest() {
	}

	/**
	 * Test first step of initialization just after an unitialization of CoIpManager.
	 * Could crash if unitialization is not correctly finished (not waiting
	 * for end of all threads) before re-initializing it.
	 */
	void testInitializationUnitialization1() {
		// A try/catch is put to catch probable crash
		BOOST_CHECK_NO_THROW({
			try {
				CoIpManagerConfig config;
				CoIpManager * coIpManager = new CoIpManager(config);
				OWSAFE_DELETE(coIpManager);
				coIpManager = new CoIpManager(config);
				OWSAFE_DELETE(coIpManager);
			} catch (...) {
				throw;
			}
		});
	}

	/**
	 * Test full initialization just after an unitialization of CoIpManager.
	 * Could crash if unitialization is not correctly finished (not waiting
	 * for end of all threads) before re-initializing it.
	 */
	void testInitializationUnitialization2() {
		CoIpManagerConfig config;
		CoIpManager * coIpManager = new CoIpManager(config);
		coIpManager->initialize();
		OWSAFE_DELETE(coIpManager);
		coIpManager = new CoIpManager(config);
		coIpManager->initialize();
		OWSAFE_DELETE(coIpManager);
	}

};

class CoIpManagerTestSuite : public test_suite {
public:

	CoIpManagerTestSuite() : test_suite("CoIpManager test suite") {
		boost::shared_ptr<CoIpManagerTest> coInstance(new CoIpManagerTest());

		test_case * coInitUninit1 = BOOST_CLASS_TEST_CASE(&CoIpManagerTest::testInitializationUnitialization1, coInstance);
		test_case * coInitUninit2 = BOOST_CLASS_TEST_CASE(&CoIpManagerTest::testInitializationUnitialization2, coInstance);

		add(coInitUninit1);
		add(coInitUninit2);
	}
};

#endif	//OWCOIPMANAGERTESTSUITE_H
