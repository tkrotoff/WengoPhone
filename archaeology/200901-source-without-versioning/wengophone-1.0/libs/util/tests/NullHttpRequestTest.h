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

#ifndef NULLHTTPREQUESTTEST_H
#define NULLHTTPREQUESTTEST_H

#include "HttpRequestTest.h"

#include <http/NullHttpRequestFactory.h>

class NullHttpRequestTest : public HttpRequestTest {
        CPPUNIT_TEST_SUITE(NullHttpRequestTest);
        CPPUNIT_TEST(testSendRequest);
        CPPUNIT_TEST(testSendRequestFailed);
        CPPUNIT_TEST_SUITE_END();

public:

	NullHttpRequestTest() : HttpRequestTest(new NullHttpRequestFactory()) {
	}

        virtual void testSendRequest() {
                HttpRequestTest::testSendRequest();
	}

        virtual void testSendRequestFailed() {
                HttpRequestTest::testSendRequestFailed();
	}
};

#endif	//NULLHTTPREQUESTTEST_H
