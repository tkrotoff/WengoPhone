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

#include <cutil/global.h>

#include "WebcamDriverTestSuite.h"

test_suite * init_unit_test_suite(int argc, char * argv[]) {
	test_suite * test = BOOST_TEST_SUITE("Webcam TestSuite");

	test->add(new WebcamDriverTestSuite());

	return test;
}

#if defined(BOOST_TEST_DYN_LINK)
bool init_unit_test_func() {
	using namespace boost::unit_test;
	framework::master_test_suite().add(
		init_unit_test_suite(framework::master_test_suite().argc,
			framework::master_test_suite().argv));
	return true;
}

int main(int argc, char* argv[]) {
	return ::boost::unit_test::unit_test_main(init_unit_test_func, argc, argv);
}
#endif
