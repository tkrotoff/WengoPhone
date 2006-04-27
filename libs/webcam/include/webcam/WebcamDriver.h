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

#ifndef WEBCAMDRIVER_H
#define WEBCAMDRIVER_H

#include <webcam/IWebcamDriver.h>

#include <pixertool/pixertool.h>

#include <cutil/global.h>

#if defined(OS_WIN32)
	class DirectXWebcamDriver;
#elif defined(OS_MACOSX)
	class QuicktimeWebcamDriver;
#elif defined(OS_LINUX)
	class V4LWebcamDriver;
#endif

class WebcamDriverFactory;

//TODO: implements a setFlags method for WEBCAM_FORCE_PALETTE_CONVERSION

/**
 *
 * @author Philippe Bernery
 */
class WebcamDriver : public IWebcamDriver {
public:

	/**
	 * Sets current factory
	 *
	 * @param factory factory to use
	 */
	static void setFactory(WebcamDriverFactory * factory) {
		_factory = factory;
	}

	/**
	 * Gets instance of WebcamDriver.
	 *
	 * Implements singleton pattern. This method create a WebcamDriver with
	 * WEBCAM_FORCE_IMAGE_FORMAT flag.
	 */
	static WebcamDriver * getInstance() {
		static WebcamDriver * instance = new WebcamDriver(WEBCAM_FORCE_IMAGE_FORMAT);
		return instance;
	}

	/** Do not use: this is an internal method. */
	void cleanup();

	void setFlags(int flags) {
		_flags |= flags;
	}

	void unsetFlags(int flags) {
		_flags &= ~flags;
	}

	bool isFlagSet(int flag) {
		//FIXME warning under Visual C++ 7.1
		return (_flags & flag);
	}

	StringList getDeviceList();

	std::string getDefaultDevice();

	webcamerrorcode setDevice(const std::string & deviceName);

	bool isOpened() const;

	void startCapture();

	void pauseCapture();

	void stopCapture();

	webcamerrorcode setPalette(pixosi palette);

	pixosi getPalette() const;

	webcamerrorcode setFPS(unsigned fps);

	unsigned getFPS() const;

	webcamerrorcode setResolution(unsigned width, unsigned height);

	unsigned getWidth() const;

	unsigned getHeight() const;

	void setBrightness(int brightness);

	int getBrightness() const;

	void setContrast(int contrast);

	int getContrast() const;

	void flipHorizontally(bool flip);

private:

	WebcamDriver(int flags);

	virtual ~WebcamDriver();

#if defined(OS_WIN32)
	friend class DirectXWebcamDriver;
#elif defined(OS_MACOSX)
	friend class QuicktimeWebcamDriver;
#elif defined(OS_LINUX)
	friend class V4LWebcamDriver;
#endif

	/**
	 * Called by platform driver when a frame is available.
	 *
	 * The frame will then be sent if the time since last frame correponds
	 * to desired fps. The fps can be managed by the software if the webcam
	 * does not support the requested fps
	 * @param image captured image
	 */
	void frameBufferAvailable(piximage * image);

	/**
	 * @see WEBCAM_FORCE_IMAGE_FORMAT
	 */
	bool isFormatForced() const {
		return _flags & WEBCAM_FORCE_IMAGE_FORMAT;
	}

	/**
	 * Initializes _convImage.
	 */
	void initializeConvImage();

	/** Pointer to factory to use. */
	static WebcamDriverFactory * _factory;

	/** Pointer to paltform dependent webcam driver. */
	IWebcamDriver * _webcamPrivate;

	/** Desired FPS. */
	unsigned _forcedFPS;

	/** Time since last frame. */
	float _fpsTimerLast;

	/**
	 * True if FPS must be forced.
	 */
	bool _forceFPS;

	/** Desired palette. */
	pixosi _desiredPalette;

	/** Desired width. */
	unsigned _desiredWidth;

	/** Desired height */
	unsigned _desiredHeight;

	/** Flags. */
	int _flags;

	/** Contains the converted captured frame. */
	piximage * _convImage;

	/** Conversion flags. */
	int _convFlags;
};

#endif	//WEBCAMDRIVER_H
