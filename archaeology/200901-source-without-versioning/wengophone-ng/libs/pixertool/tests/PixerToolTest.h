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

#ifndef OWPIXERTOOLTEST_H
#define OWPIXERTOOLTEST_H

#include <cutil/global.h>

#ifdef OS_MACOSX
	#include <SDL_image/SDL_image.h>
#else
	#include <SDL_image.h>
#endif

#include <pixertool/pixertool.h>
#include <pixertool/ffmpeg-pixertool.h>

#include <boost/test/unit_test.hpp>
using boost::unit_test_framework::test_suite;
using boost::unit_test_framework::test_case;

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

class PixerToolTest {
public:

	PixerToolTest() {
		palettes.push_back(PIX_OSI_YUV422P);
		palettes.push_back(PIX_OSI_YUV444P);
		palettes.push_back(PIX_OSI_YUV422);
		palettes.push_back(PIX_OSI_YUV411);
		palettes.push_back(PIX_OSI_YUV424P);
		palettes.push_back(PIX_OSI_YUV41P);
		palettes.push_back(PIX_OSI_YUY2);
		palettes.push_back(PIX_OSI_YUYV);
		palettes.push_back(PIX_OSI_YVYU);
		palettes.push_back(PIX_OSI_UYVY);
		palettes.push_back(PIX_OSI_YV12);
		palettes.push_back(PIX_OSI_UNSUPPORTED);
		palettes.push_back(PIX_OSI_YUV420P);
		palettes.push_back(PIX_OSI_I420);
		palettes.push_back(PIX_OSI_RGB555);
		palettes.push_back(PIX_OSI_RGB565);
		palettes.push_back(PIX_OSI_RGB1);
		palettes.push_back(PIX_OSI_RGB4);
		palettes.push_back(PIX_OSI_RGB8);
		palettes.push_back(PIX_OSI_RGB32);
		palettes.push_back(PIX_OSI_RGB24);
		palettes.push_back(PIX_OSI_NV12);

		flags.push_back(PIX_NO_FLAG);
		flags.push_back(PIX_FLIP_HORIZONTALLY);
	}

	void testIdConversion() {
	}

	void testAllocFree() {
		for (vector<pixosi>::const_iterator curPalette = palettes.begin(); curPalette != palettes.end(); curPalette++) {
			if (pix_ffmpeg_from_pix_osi(*curPalette) != PIX_OSI_UNSUPPORTED) {
				piximage * image = pix_alloc(*curPalette, 320, 240);
				BOOST_CHECK_MESSAGE(image != NULL, "Error allocating 320x240 " << pixosiToString(*curPalette) << " picture");

				pix_free(image);
			}
		}
	}

	/**
	 * Check data conversion.
	 *
	 * Check data conversion from a RGB24 picture to every available format
	 * with every available flags
	 */
	void testDataConversion() {
		SDL_Surface * image = IMG_Load("test.bmp");
		BOOST_REQUIRE_MESSAGE(image != NULL, "Error while loading test picture: " << IMG_GetError());

		SDL_LockSurface(image);

		pixosi srcPalette = PIX_OSI_RGB24;
		piximage src;
		src.palette = srcPalette;
		src.width = image->w;
		src.height = image->h;
		src.data = (uint8_t *) image->pixels;

		for (vector<pixosi>::const_iterator curPalette = palettes.begin(); curPalette != palettes.end(); curPalette++) {

			if (pix_ffmpeg_from_pix_osi(*curPalette) != PIX_OSI_UNSUPPORTED) {

				for (vector<int>::const_iterator curFlag = flags.begin(); curFlag != flags.end(); curFlag++) {

					piximage * dst1 = pix_alloc(*curPalette, 320, 240);
					BOOST_CHECK_MESSAGE(dst1 != NULL, "Error allocating 320x240 " << pixosiToString(*curPalette) << " picture");

					pixerrorcode ret = pix_convert(*curFlag, dst1, &src);
					BOOST_CHECK_MESSAGE(ret != PIX_OK, "Palette not supported");

					piximage * dst2 = pix_alloc(src.palette, 320, 240);
					BOOST_CHECK_MESSAGE(dst2 != NULL, "Error allocating 320x240 " << pixosiToString(*curPalette) << " picture");

					ret = pix_convert(*curFlag, dst2, dst1);
					BOOST_CHECK_MESSAGE(ret != PIX_OK, "Palette not supported");

					cout << "Checking conversion from=" << pixosiToString(src.palette)
						<< " to=" << pixosiToString(*curPalette)
						<< " with " << ((*curFlag == PIX_FLIP_HORIZONTALLY) ? "horizontal flip" : "no flag")
						<< endl;

					BOOST_CHECK(memcmp(src.data, dst2->data, pix_size(srcPalette, image->w, image->h)) == 0);

					pix_free(dst1);
					pix_free(dst2);

				}
			}
		}

		SDL_UnlockSurface(image);
		SDL_FreeSurface(image);
	}

private:

	string pixosiToString(pixosi format) {
		string result;

		switch (format) {
		case PIX_OSI_YUV420P:
			result = "yuv420p";
			break;
		case PIX_OSI_I420:
			result = "i420";
			break;
		case PIX_OSI_YUV422:
			result = "yuv422";
			break;
		case PIX_OSI_YUV411:
			result = "yuv411";
			break;
		case PIX_OSI_YUV422P:
			result = "yuv422p";
			break;
		case PIX_OSI_YUV444P:
			result = "yuv444p";
			break;
		case PIX_OSI_YUV424P:
			result = "yuv424p";
			break;
		case PIX_OSI_YUV41P:
			result = "yuv41p";
			break;
		case PIX_OSI_YUY2:
			result = "yuy2";
			break;
		case PIX_OSI_YUYV:
			result = "yuyv";
			break;
		case PIX_OSI_YVYU:
			result = "yvyu";
			break;
		case PIX_OSI_UYVY:
			result = "uyvy";
			break;
		case PIX_OSI_YV12:
			result = "yv12";
			break;
		case PIX_OSI_RGB555:
			result = "rgb555";
			break;
		case PIX_OSI_RGB565:
			result = "rgb565";
			break;
		case PIX_OSI_RGB1:
			result = "rgb1";
			break;
		case PIX_OSI_RGB4:
			result = "rgb4";
			break;
		case PIX_OSI_RGB8:
			result = "rgb8";
			break;
		case PIX_OSI_RGB32:
			result = "rgb32";
			break;
		case PIX_OSI_RGB24:
			result = "rgb24";
			break;
		case PIX_OSI_NV12:
			result = "nv12";
			break;
		case PIX_OSI_UNSUPPORTED:
		default:
			result = "does not exist";
		}

		return result;
	}

	vector<pixosi> palettes;

	vector<int> flags;
};

class PixerToolTestSuite : public test_suite {
public:

	PixerToolTestSuite()
		: test_suite("PixerToolTestSuite") {

		boost::shared_ptr<PixerToolTest> instance(new PixerToolTest());

		test_case * testIdConversion = BOOST_CLASS_TEST_CASE(&PixerToolTest::testIdConversion, instance);
		test_case * testAllocFree = BOOST_CLASS_TEST_CASE(&PixerToolTest::testAllocFree, instance);
		test_case * testDataConversion = BOOST_CLASS_TEST_CASE(&PixerToolTest::testDataConversion, instance);

		add(testIdConversion);
		add(testAllocFree);
		add(testDataConversion);
	}
};

#endif	//OWPIXERTOOLTEST_H
