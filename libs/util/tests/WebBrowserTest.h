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

#ifndef WEBBROWSERTEST_H
#define WEBBROWSERTEST_H

#include <WebBrowser.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

class WebBrowserTest {
public:

	void testOpenUrl() {
		BOOST_CHECK(WebBrowser::openUrl("http://wengo.fr"));
	}
};

class WebBrowserTestSuite : public test_suite {
public:

	WebBrowserTestSuite() : test_suite("WebBrowserTestSuite") {
		boost::shared_ptr<WebBrowserTest> instance(new WebBrowserTest());

		test_case * testOpenUrl = BOOST_CLASS_TEST_CASE(&WebBrowserTest::testOpenUrl, instance);

		add(testOpenUrl);
	}
};

#endif	//WEBBROWSERTEST_H
