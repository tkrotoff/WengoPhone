/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWWEBCAMDRIVERTESTSUITE_H
#define OWWEBCAMDRIVERTESTSUITE_H

#include <webcam/WebcamDriver.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

#include <iostream>
using namespace std;

class WebcamDriverTest {
public:

	WebcamDriverTest() {
		driver = WebcamDriver::getInstance();
	}

	void testDeviceList() {
		StringList list = driver->getDeviceList();
		BOOST_CHECK(list.size() == 0);
	}

	void testNameOfDefaultDevice() {
		cout << driver->getDefaultDevice() << endl;
	}

private:

	WebcamDriver * driver;
};

class WebcamDriverTestSuite : public test_suite {
public:

	WebcamDriverTestSuite()
		: test_suite("WebcamDriverTestSuite") {

		boost::shared_ptr<WebcamDriverTest> instance(new WebcamDriverTest());

		test_case * testDeviceList = BOOST_CLASS_TEST_CASE(&WebcamDriverTest::testDeviceList, instance);
		test_case * testNameOfDefaultDevice = BOOST_CLASS_TEST_CASE(&WebcamDriverTest::testNameOfDefaultDevice, instance);

		add(testDeviceList);
		add(testNameOfDefaultDevice);
	}
};

#endif	//OWWEBCAMDRIVERTESTSUITE_H
