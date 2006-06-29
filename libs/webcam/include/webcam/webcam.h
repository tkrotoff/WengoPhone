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

#ifndef WEBCAM_H
#define WEBCAM_H

#include <pixertool/pixertool.h>

/*TODO: add function to set factory*/

/**
 * C interface to OS independent webcam controller.
 *
 * @file webcam.h
 * @author David Ferlier
 * @author Mathieu Stute
 * @author Philippe Bernery
 */

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

typedef enum {
	WEBCAM_NOK,
	WEBCAM_OK
} webcamerrorcode;

typedef struct {
	unsigned count;
	const char ** name;
} webcamdevicelist;

/**
 * Name returned by get_default_device when there is no webcam.
 */
#define WEBCAM_NULL ""

/**
 * The flag says that we want the webcam driver to convert the captured
 * picture to the desired format if the webcam does not support the desired
 * format. If so, the webcam driver will use a software fallback.
 */
#define WEBCAM_FORCE_IMAGE_FORMAT 1


/** Contains access to C++ code */
struct opaquewebcam;
typedef struct opaquewebcam webcam;

/**
 * Frame captured callback type.
 *
 * @param sender sender of image
 * @param image captured image
 * @param userData user data
 */
typedef void (webcamcallback)(webcam * sender, piximage * image, void * userData);

/**
 * @see IWebcamDriver::getDeviceList()
 */
webcamdevicelist * webcam_get_device_list(webcam *);

/**
 * Release memory used by a webcamdevicelist.
 *
 * @param devlist list to release
 */
void webcam_release_webcamdevicelist(webcamdevicelist * devlist);

/**
 * @see IWebcamDriver::getDefaultDevice()
 */
const char * webcam_get_default_device(webcam * wc);

/**
 * Gets instance of webcam.
 *
 * Webcam is a singleton
 *
 * @return the new webcam
 */
webcam * webcam_get_instance();

/**
 * @see IWebcamDriver::setFlags(int flags)
 */
void webcam_set_flags(webcam * wc, int flags);

/**
 * @see IWebcamDriver::unsetFlags()
 */
void webcam_unset_flags(webcam * wc, int flags);

/**
 * @see IWebcamDriver::isFlagSet()
 */
int webcam_is_flag_set(webcam * wc, int flag);

/**
 * Release webcam.
 *
 */
void webcam_release(webcam * wc);

/**
 * @see IWebcamDriver::setDevice()
 */
webcamerrorcode webcam_set_device(webcam * wc, const char * device_name);

/**
 * Adds a callback.
 *
 * The callbacks are called when a new frame has been captured.
 *
 * @param callback the callback to register
 * @param userData user data to send to callback
 */
void webcam_add_callback(webcam * wc, webcamcallback * callback, void * userData);

/**
 * Removes the callback from callback list.
 *
 * @param callback the callback to unregister
 */
void webcam_remove_callback(webcam * wc, webcamcallback * callback);

/**
 * @see IWebcamDriver::startCapture()
 */
void webcam_start_capture(webcam * wc);

/**
 * @see IWebcamDriver::pauseCapture()
 */
void webcam_pause_capture(webcam * wc);

/**
 * @see IWebcamDriver::stopCapture()
 */
void webcam_stop_capture(webcam * wc);

/**
 * @see IWebcamDriver::getWidth()
 */
unsigned webcam_get_width(webcam * wc);

/**
 * @see IWebcamDriver::getHeight()
 */
unsigned webcam_get_height(webcam * wc);

/**
 * @see IWebcamDriver::getPalette()
 */
pixosi webcam_get_palette(webcam * wc);

/**
 * @see IWebcamDriver::setPalette()
 */
webcamerrorcode webcam_set_palette(webcam * wc, pixosi palette);

/**
 * @see IWebcamDriver::isOpen()
 */
int webcam_is_open(webcam * wc);

/**
 * @see IWebcamDriver::setFPS()
 */
webcamerrorcode webcam_set_fps(webcam * wc, unsigned fps);

/**
 * @see IWebcamDriver::getFPS()
 */
unsigned webcam_get_fps(webcam * wc);

/**
 * @see IWebcamDriver::setResolution()
 */
void webcam_set_resolution(webcam * wc, unsigned width, unsigned height);

/**
 * @see IWebcamDriver::flipHorizontally()
 */
void webcam_flip_horizontally(webcam * wc, int flip);

/**
 * @see IWebcamDriver::setBrightness()
 */
void webcam_set_brightness(webcam * wc, int brightness);

/**
 * @see IWebcamDriver::getBrightness()
 */
int webcam_get_brightness(webcam * wc);

/**
 * @see IWebcamDriver::setContrast()
 */
void webcam_set_contrast(webcam * wc, int contrast);

/**
 * @see IWebcamDriver::getContrast()
 */
int webcam_get_contrast(webcam * wc);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif	/*WEBCAM_H*/
