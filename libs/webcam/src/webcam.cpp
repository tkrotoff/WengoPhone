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

#include <webcam/webcam.h>

#include <webcam/WebcamDriver.h>

using namespace std;

typedef struct _webcamcallbacklist {
	/** sender */
	webcam * wc;

	/** The callback */
	webcamcallback * cbk;

	/** User data associated with this callback */
	void * data;

	/** Link to previous and next callback. Linked list */
	_webcamcallbacklist *prev, *next;

} webcamcallbacklist;

struct opaquewebcam {
	WebcamDriver * driver;
};

//FIXME: there is only one list for all created webcam struct.
static webcamcallbacklist * cbks = NULL;

static void webcam_frame_captured_event_handler(IWebcamDriver *sender, piximage *image) {
	webcamcallbacklist * cur = cbks;

	while (cur) {
		cur->cbk(cur->wc, image, cur->data);
		cur = cur->next;
	}
}

webcamdevicelist * webcam_get_device_list(webcam *) {
	IWebcamDriver * driver = WebcamDriver::getInstance();
	StringList deviceList = driver->getDeviceList();
	webcamdevicelist *devlist = (webcamdevicelist *) malloc(sizeof(webcamdevicelist));

	devlist->count = deviceList.size();
	devlist->name = (const char **) malloc(devlist->count * sizeof(const char *));

	for (register unsigned i = 0; i < devlist->count; i++) {
		devlist->name[i] = (const char *) malloc(deviceList[i].size());
		sprintf((char *) devlist->name[i], deviceList[i].c_str(), deviceList[i].size());
	}

	return devlist;
}

void webcam_release_webcamdevicelist(webcamdevicelist *devlist) {
	if (devlist) {
		if (devlist->name) {
			for (register unsigned i = 0; i < devlist->count; i++) {
				if (devlist->name[i]) {
					free((void *) devlist->name[i]);
				}
			}
			free(devlist->name);
		}
		free(devlist);
	}
}

const char * webcam_get_default_device(webcam *) {
	IWebcamDriver * driver = WebcamDriver::getInstance();
	static string device;

	device = driver->getDefaultDevice();

	return device.c_str();
}

webcam * webcam_get_instance() {
	webcam * wc = (webcam *) malloc(sizeof(webcam));

	wc->driver = WebcamDriver::getInstance();
	wc->driver->frameCapturedEvent += &webcam_frame_captured_event_handler;

	return wc;
}

void webcam_set_flags(webcam * wc, int flags) {
	wc->driver->setFlags(flags);
}

void webcam_unset_flags(webcam * wc, int flags) {
	wc->driver->unsetFlags(flags);
}

int webcam_is_flag_set(webcam * wc, int flag) {
	return wc->driver->isFlagSet(flag);
}

void webcam_release(webcam * wc) {
	webcamcallbacklist *nxt, *cur;

	webcam_stop_capture(wc);
	if (wc) {
		free(wc);

		cur = cbks;
		while (cur) {
			nxt = cur->next;
			free(cur);
			cur = nxt;
		}
		cbks = NULL;
	}
}

webcamerrorcode webcam_set_device(webcam * wc, const char * device_name) {
	return wc->driver->setDevice(device_name);
}

void webcam_add_callback(webcam * wc, webcamcallback * callback, void * userData) {
	webcamcallbacklist * cur;

	if (cbks == NULL) {
		cbks = (webcamcallbacklist *) malloc(sizeof(webcamcallbacklist));
		cbks->prev = NULL;
		cbks->next = NULL;
		cur = cbks;
	} else {
		//Find last
		cur = cbks;
		while (cur->next != NULL) {
			cur = cur->next;
		}

		cur->next = (webcamcallbacklist *) malloc(sizeof(webcamcallbacklist));
		cur->next->prev = cur;
		cur->next->next = NULL;
	}

	cur->wc = wc;
	cur->cbk = callback;
	cur->data = userData;
}

void webcam_remove_callback(webcam * wc, webcamcallback * callback) {
	webcamcallbacklist * cur = cbks;

	while ((cur != NULL) && (cur->cbk != callback)) {
		cur = cur->next;
	}

	if (cur != NULL) {
		cur->prev->next = cur->next;
		if (cur->next) {
			cur->next->prev = cur->prev;
		}
		free(cur);
	}
}

void webcam_start_capture(webcam * wc) {
	wc->driver->startCapture();
}

void webcam_pause_capture(webcam * wc) {
	wc->driver->pauseCapture();
}

void webcam_stop_capture(webcam * wc) {
	wc->driver->stopCapture();
}

unsigned webcam_get_width(webcam * wc) {
	return wc->driver->getWidth();
}

unsigned webcam_get_height(webcam * wc) {
	return wc->driver->getHeight();
}

pixosi webcam_get_palette(webcam * wc) {
	return wc->driver->getPalette();
}

webcamerrorcode webcam_set_palette(webcam * wc, pixosi palette) {
	return wc->driver->setPalette(palette);
}

int webcam_is_open(webcam * wc) {
	return wc->driver->isOpen();
}

webcamerrorcode webcam_set_fps(webcam * wc, unsigned fps) {
	return wc->driver->setFPS(fps);
}

unsigned webcam_get_fps(webcam * wc) {
	return wc->driver->getFPS();
}

void webcam_set_resolution(webcam * wc, unsigned width, unsigned height) {
	wc->driver->setResolution(width, height);
}

void webcam_flip_horizontally(webcam * wc, int flip) {
	wc->driver->flipHorizontally(flip);
}

void webcam_set_brightness(webcam * wc, int brightness) {
	wc->driver->setBrightness(brightness);
}

int webcam_get_brightness(webcam * wc) {
	return wc->driver->getBrightness();
}

void webcam_set_contrast(webcam * wc, int contrast) {
	wc->driver->setContrast(contrast);
}

int webcam_get_contrast(webcam * wc) {
	return wc->driver->getContrast();
}
