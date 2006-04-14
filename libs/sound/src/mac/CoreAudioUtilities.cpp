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

#include "CoreAudioUtilities.h"

#include <util/Logger.h>

std::vector<AudioDeviceID> CoreAudioUtilities::allAudioDeviceList() {
	OSStatus status = noErr;
	UInt32 numberOfDevices = 0;
	UInt32 deviceListSize = 0;
	AudioDeviceID * deviceList = NULL;
	std::vector<AudioDeviceID> result;

	// Getting number of devices
	status = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &deviceListSize, NULL);
	if (status) {
		LOG_ERROR("Can't get property info: kAudioHardwarePropertyDevices");
		return result;
	}

	numberOfDevices = deviceListSize / sizeof(AudioDeviceID);
	////

	// Getting device list
	deviceList = (AudioDeviceID *) calloc(sizeof(AudioDeviceID), deviceListSize);

	status = AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &deviceListSize, deviceList);
	if (status) {
		LOG_ERROR("Can't get property: kAudioHardwarePropertyDevices");
		return result;
	}
	////

	// Populating the result
	for (unsigned i = 0 ; i < numberOfDevices ; ++i) {
		result.push_back(deviceList[i]);
	}
	////

	return result;
}

std::string CoreAudioUtilities::audioDeviceName(AudioDeviceID id, bool isInput) {
	OSStatus status = noErr;
	char deviceName[1024];
	UInt32 size = sizeof(deviceName);
	memset(deviceName, 0, sizeof(deviceName));
	std::string result;

	status = AudioDeviceGetProperty(id, 0, (isInput ? TRUE : FALSE), kAudioDevicePropertyDeviceName, &size, deviceName);
	if (status) {
		LOG_ERROR("Can't get device property: kAudioDevicePropertyDeviceName");
		return result;
	}

	result = deviceName;

	return result;
}

std::map<AudioDeviceID, std::string> CoreAudioUtilities::audioDeviceMap(bool isInput) {
	std::map<AudioDeviceID, std::string> result;

	std::vector<AudioDeviceID> deviceId = allAudioDeviceList();
	for (std::vector<AudioDeviceID>::const_iterator it = deviceId.begin();
		it != deviceId.end();
		++it) {
		std::string deviceName = audioDeviceName(*it, isInput);
		if (!deviceName.empty()) {
			result[*it] = deviceName;
		}
	}

	return result;
}
