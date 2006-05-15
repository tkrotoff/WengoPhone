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

#include <webcam/WebcamDriver.h>

#include <webcam/DefaultWebcamDriverFactory.h>

#include <util/Logger.h>

WebcamDriverFactory * WebcamDriver::_factory = 0;

#include <iostream>
using namespace std;

WebcamDriver::WebcamDriver(int flags)
: IWebcamDriver(flags) {
	if (!_factory) {
		_factory = new DefaultWebcamDriverFactory();
	}

	_webcamPrivate = _factory->create(this, flags);

	_convImage = NULL;

	cleanup();

	_forceFPS = false;
	_forcedFPS = 15;
	_fpsTimerLast = 0;

	_desiredPalette = PIX_OSI_YUV420P;
	_desiredWidth = 320;//176;
	_desiredHeight = 240;//144;

	_flags = flags;

	_convFlags = PIX_NO_FLAG;
}

WebcamDriver::~WebcamDriver() {
	stopCapture();
	delete _webcamPrivate;

	if (_convImage)
		pix_free(_convImage);
}

void WebcamDriver::cleanup() {
	LOG_DEBUG("Cleaning up the webcam driver");
	_webcamPrivate->cleanup();

	initializeConvImage();
}

StringList WebcamDriver::getDeviceList() {
	return _webcamPrivate->getDeviceList();
}

std::string WebcamDriver::getDefaultDevice() {
	return _webcamPrivate->getDefaultDevice();
}

webcamerrorcode WebcamDriver::setDevice(const std::string & deviceName) {
	cleanup();

	std::string actualDeviceName = deviceName;
	if (actualDeviceName.empty()) {
		actualDeviceName = getDefaultDevice();
	}

	LOG_DEBUG("desired device: " + deviceName + ", actual device: " + actualDeviceName);
	return _webcamPrivate->setDevice(actualDeviceName);
}

bool WebcamDriver::isOpened() const {
	return _webcamPrivate->isOpened();
}

void WebcamDriver::startCapture() {
	LOG_DEBUG("Starting capture");
	_webcamPrivate->startCapture();
}

void WebcamDriver::pauseCapture() {
	LOG_DEBUG("Pausing capture");
	_webcamPrivate->pauseCapture();
}

void WebcamDriver::stopCapture() {
	LOG_DEBUG("Stopping capture");
	_webcamPrivate->stopCapture();
	cleanup();
}

webcamerrorcode WebcamDriver::setPalette(pixosi palette) {
	if (_webcamPrivate->setPalette(palette) == WEBCAM_NOK) {
		LOG_DEBUG("This webcam does not support palette #" + String::fromNumber(palette));
		if (isFormatForced()) {
			LOG_DEBUG("Palette conversion will be forced");
			_desiredPalette = palette;
			initializeConvImage();
			return WEBCAM_OK;
		} else {
			return WEBCAM_NOK;
		}
	} else {
		LOG_DEBUG("This webcam supports palette #" + String::fromNumber(palette));
		_desiredPalette = palette;
		return WEBCAM_OK;
	}
}

pixosi WebcamDriver::getPalette() const {
	if (isFormatForced()) {
		return _desiredPalette;
	} else {
		return _webcamPrivate->getPalette();
	}
}

webcamerrorcode WebcamDriver::setFPS(unsigned fps) {
	if (_webcamPrivate->setFPS(fps) == WEBCAM_NOK) {
		LOG_DEBUG("This webcam does not support the desired fps(" + String::fromNumber(fps) + "). Will force it");
		_forceFPS = true;
	} else {
		LOG_DEBUG("Webcam FPS changed to " + String::fromNumber(fps));
		_forceFPS = false;
	}

	_forcedFPS = fps;

	return WEBCAM_OK;
}

unsigned WebcamDriver::getFPS() const {
	return _forcedFPS;
}

webcamerrorcode WebcamDriver::setResolution(unsigned width, unsigned height) {
	LOG_DEBUG("try to change resolution: (width, height) = " + String::fromNumber(width) + "," + String::fromNumber(height));
	if (_webcamPrivate->setResolution(width, height) == WEBCAM_NOK) {
		if (isFormatForced()) {
			_desiredWidth = width;
			_desiredHeight = height;
			initializeConvImage();
			return WEBCAM_OK;
		} else {
			return WEBCAM_NOK;
		}
	} else {
		_desiredWidth = width;
		_desiredHeight = height;
		return WEBCAM_OK;
	}
}

unsigned WebcamDriver::getWidth() const {
	if (isFormatForced()) {
		return _desiredWidth;
	} else {
		return _webcamPrivate->getWidth();
	}
}

unsigned WebcamDriver::getHeight() const {
	if (isFormatForced()) {
		return _desiredHeight;
	} else {
		return _webcamPrivate->getHeight();
	}
}

void WebcamDriver::setBrightness(int brightness) {
	_webcamPrivate->setBrightness(brightness);
}

int WebcamDriver::getBrightness() const {
	return _webcamPrivate->getBrightness();
}

void WebcamDriver::setContrast(int contrast) {
	_webcamPrivate->setContrast(contrast);
}

int WebcamDriver::getContrast() const {
	return _webcamPrivate->getContrast();
}

void WebcamDriver::flipHorizontally(bool flip) {
	if (flip) {
		LOG_DEBUG("enable horizontal flip");
		_convFlags |= PIX_FLIP_HORIZONTALLY;
	} else {
		LOG_DEBUG("disable horizontal flip");
		_convFlags &= ~PIX_FLIP_HORIZONTALLY;
	}
}

void WebcamDriver::frameBufferAvailable(piximage *image) {
	float now;
	float fpsTimerInter = 1000 / (float)_forcedFPS;

#ifdef CC_MSVC
	now = timeGetTime();
#else
	now = clock() / ((float)CLOCKS_PER_SEC / 1000.0);
#endif
	if (_forceFPS) {
		if ((now - _fpsTimerLast) < fpsTimerInter) {
			return;
		}
		_fpsTimerLast = now;
	}

	if ((isFormatForced() && ((_desiredPalette != image->palette)
			|| (_desiredWidth != image->width) || (_desiredHeight != image->height)))
		|| (_convFlags != PIX_NO_FLAG)) {
		LOG_DEBUG("conversion needed: from palette #" + String::fromNumber(image->palette)
			+ " to palette #" + String::fromNumber(_desiredPalette)
			+ " and from (" + String::fromNumber(image->width) + "x" + String::fromNumber(image->height)
			+ ") to (" + String::fromNumber(_desiredWidth) + "x" + String::fromNumber(_desiredHeight) + ")");

		pix_convert(_convFlags, _convImage, image);

		frameCapturedEvent(this, _convImage);
	} else {
		//LOG_DEBUG("no conversion needed");
		frameCapturedEvent(this, image);
	}
}

void WebcamDriver::initializeConvImage() {
	if (_convImage) {
		pix_free(_convImage);
	}

	_convImage = pix_alloc(_desiredPalette, _desiredWidth, _desiredHeight);
}
