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

#include "Video.h"

#include <observer/Event.h>
#include "connect/Connect.h"

#include <webcam/WebcamDevice.h>

#include <qstring.h>

const QString Video::VIDEO_TAG = "video";
const QString Video::NAME_TAG = "name";
const QString Video::FLIP_TAG = "flip";
const QString Video::QUALITY_TAG = "quality";
const QString Video::FRAMERATE_TAG = "frameRate";
const QString Video::RESOLUTION_TAG = "resolution";
const QString Video::INPUTDEVICE_TAG = "inputDevice";
const QString Video::VIDEOENABLE_TAG = "video-enable";
const QString Video::CONNECTIONTYPE_TAG = "connectionType";

Video::Video() {
	_enable = true;
	_resolution = "176x144";
	_frameRate = "15";
	_quality = "50";
	_flip = false;
	_connectionType = "128";
	_inputDeviceName = WebcamDevice::getDefaultDevice();
}

/**
* @param inputDeviceName
* @brief set input device name
*/
void Video::setInputDevice(const QString & inputDeviceName) {
	if (WebcamDevice::getWebcamList().contains(inputDeviceName) > 0) {
		_inputDeviceName = inputDeviceName;
		//notify(Event());
	}
}

/**
* @return input device name
*/
const QString & Video::getInputDeviceName() const {
	return _inputDeviceName;
}

int Video::getInputDeviceId() const {
//	return WebcamDevice::getMixerDeviceId(_inputDeviceName);
	return 0;
}

/**
 * @param inputDeviceName
 * @brief change all attributes
 */
void Video::changeSettings(const QString & inputDeviceName) {
	_inputDeviceName = inputDeviceName;
	notify(Event());
	Connect::getInstance().connect();
}

/**
 * @brief set default settings
 */
void Video::defaultSettings() {
	_enable = true;
	_resolution = "176x144";
	_frameRate = "15";
	_quality = "50";
	_flip = false;
	_connectionType = "128";
	changeSettings(WebcamDevice::getDefaultDevice());
}
