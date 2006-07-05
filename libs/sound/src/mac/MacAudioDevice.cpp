/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "MacAudioDevice.h"

#include "CoreAudioUtilities.h"

#include <util/Logger.h>
#include <util/String.h>

const std::string MacAudioDevice::SEPARATOR = ":";

MacAudioDevice::MacAudioDevice(const std::string & deviceId) 
: AudioDevice() {
	unserialize(deviceId);
}

MacAudioDevice::MacAudioDevice(AudioDeviceID audioDeviceID, UInt32 dataSourceID, bool isInput) {
	_audioDeviceID = audioDeviceID;
	_dataSourceID = dataSourceID;
	_isInput = isInput;
}

MacAudioDevice::MacAudioDevice(AudioDeviceID audioDeviceID, bool isInput) {
	_audioDeviceID = audioDeviceID;
	_dataSourceID = 0;
	_isInput = isInput;
}

MacAudioDevice::~MacAudioDevice() {
}

std::string MacAudioDevice::getName() const {
	std::string result;

	std::string deviceName = CoreAudioUtilities::audioDeviceName(_audioDeviceID, _isInput);

	if (_dataSourceID != 0) {
		std::string dataSourceName = CoreAudioUtilities::dataSourceName(_audioDeviceID, _isInput, _dataSourceID);
		result = deviceName + " - " + dataSourceName;
	} else {
		result = deviceName;
	}

	return result;
}

std::string MacAudioDevice::getId() const {
	return serialize();
}

void MacAudioDevice::unserialize(const std::string & data) {
	std::string::size_type devIndex = data.find(SEPARATOR);
	if (devIndex != std::string::npos) {
		String deviceId = data.substr(0, devIndex);

		std::string::size_type dsIndex = data.find(SEPARATOR, devIndex + 1);
		if (dsIndex != std::string::npos) {
			String dataSourceId = data.substr(devIndex + 1, dsIndex);
			String isInput = data.substr(dsIndex + 1);

			_audioDeviceID = deviceId.toInteger();
			_dataSourceID = dataSourceId.toInteger();
			_isInput = isInput.toInteger();
		} else {
			LOG_ERROR("unserialization error: cannot get data source id");
		}
	} else {
		LOG_ERROR("unserialization error: cannot get device id");	
	}
}

std::string MacAudioDevice::serialize() const {
	std::string result;

	result += String::fromNumber(_audioDeviceID);
	result += SEPARATOR;
	result += String::fromNumber(_dataSourceID);
	result += SEPARATOR;
	result += String::fromNumber(_isInput);
	
	return result;
}
