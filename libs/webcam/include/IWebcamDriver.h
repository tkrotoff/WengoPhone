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

#ifndef IWEBCAMDRIVER_H
#define IWEBCAMDRIVER_H

#include "webcam.h"
#include <pixertool.h>
#include <Event.h>
#include <NonCopyable.h>
#include <StringList.h>
#include <string>

/**
 * Drive a webcam device
 *
 * @author Philippe BERNERY
 */
class IWebcamDriver : NonCopyable {
public:

	/**
	 * Get webcam device list
	 */
	virtual StringList getDeviceList() = 0;

	/**
	 * Get default webcam device
	 *
	 * @return the name of the device. WEBCAM_NULL if no webcam or error.
	 */
	virtual std::string getDefaultDevice() = 0;

	/**
	 * A frame has been captured
	 *
	 * @param sender sender of the event
	 * @param image captured image
	 */
	Event<void (IWebcamDriver *sender, piximage *image)> frameCapturedEvent;

	/**
	 * @param flags set the webcam driver flags
	 */
	IWebcamDriver(int flags) { }

	virtual ~IWebcamDriver() { }

	/**
	 * Initializes/cleanups private member variables.
	 */
	virtual void cleanup() = 0;

	/**
	 * Set flags.
	 *
	 * @param flags OR_bitwise flags to set
	 * @see webcam.h for documentation about valid flags
	 */
	virtual void setFlags(int flags) = 0;

	/**
	 * Unset flags.
	 *
	 * @param flags OR-bitwise flags to unset
	 */
	virtual void unsetFlags(int flags) = 0;

	/**
	 * Is given flag set?
	 *
	 * @param flag flag to check
	 * @return true if set
	 */
	virtual bool isFlagSet(int flag) = 0;

	/**
	 * Set webcam device to use
	 *
	 * @param deviceName name of the device to use, if name == "" then use the default device
	 * @return WEBCAM_OK if no error, WEBCAM_NOK otherwise
	 */
	virtual webcamerrorcode setDevice(const std::string & deviceName) = 0;

	/**
	 * @return true webcam if is opened
	 */
	virtual bool isOpened() const = 0;

	/**
	 * Start capture of video frames
	 */
	virtual void startCapture() = 0;

	/**
	 * Pause capture of video frames
	 */
	virtual void pauseCapture() = 0;

	/**
	 * Stop capture of video frames
	 */
	virtual void stopCapture() = 0;

	/**
	 * Set palette
	 *
	 * @param palette palette to use
	 */
	virtual webcamerrorcode setPalette(pixosi palette) = 0;

	/**
	 * Get palette
	 *
	 * @return current palette
	 */
	virtual pixosi getPalette() const = 0;

	/**
	 * Set capture rate
	 *
	 * @param fps desired frame per seconds
	 */
	virtual webcamerrorcode setFPS(unsigned fps) = 0;

	/**
	 * Get capture rate
	 *
	 * @return current frame per seconds
	 */
	virtual unsigned getFPS() const = 0;

	/**
	 * Set capture resolution
	 *
	 * @param width desired width
	 * @param height desired height
	 */
	virtual webcamerrorcode setResolution(unsigned width, unsigned height) = 0;

	/**
	 * Get captured frame width
	 *
	 * @return captured frame width
	 */
	virtual unsigned getWidth() const = 0;

	/**
	 * Get captured frame height
	 *
	 * @return captured frame height
	 */
	virtual unsigned getHeight() const = 0;

	/**
	 * Set capture brightness
	 *
	 * @param brightness brightness value
	 */
	virtual void setBrightness(int brightness) = 0;

	/**
	 * Get capture brightness
	 *
	 * @param brightness brightness value
	 */
	virtual int getBrightness() const = 0;

	/**
	 * Set capture contrast
	 *
	 * @param contrast contrast value
	 */
	virtual void setContrast(int contrast) = 0;

	/**
	 * Get capture contrast
	 *
	 * @param contrast contrast value
	 */
	virtual int getContrast() const = 0;

	/**
	 * Flip captured frame horizontally
	 *
	 * @param flip if true the picture will be flipped horizontally
	 */
	virtual void flipHorizontally(bool flip) = 0;
};

#endif //IWEBCAMDRIVER_H

