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
	memset(deviceName, 0, sizeof(deviceName));
	UInt32 size = sizeof(deviceName);
	Boolean input = (isInput ? TRUE : FALSE);
	std::string result;

	status = AudioDeviceGetProperty(id, 0, input, kAudioDevicePropertyDeviceName, &size, deviceName);
	if (status) {
		LOG_ERROR("Can't get device property: kAudioDevicePropertyDeviceName");
		return result;
	}

	status = AudioDeviceGetPropertyInfo(id, 0, input, kAudioDevicePropertyStreamConfiguration, &size, NULL);
	if (status) {
		LOG_ERROR("Can't get device property info: kAudioDevicePropertyStreamConfiguration");
		return result;
	}

	AudioBufferList list;
	//FIXME: should get the size 'size'
	size = sizeof(list);
	status = AudioDeviceGetProperty(id, 0, input, kAudioDevicePropertyStreamConfiguration, &size, &list);
	if (status) {
		LOG_INFO("Can't get device property: kAudioDevicePropertyStreamConfiguration."
			" The device has no " + (isInput ? std::string("input") : std::string("output")) + " device.");
		return result;
	}

	for (unsigned i = 0; i < list.mNumberBuffers; ++i) {
		if (list.mBuffers[i].mNumberChannels > 0) {
			result = deviceName;
			break;
		}
	}

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

std::vector<UInt32> CoreAudioUtilities::dataSourcesList(AudioDeviceID id, bool isInput) {
	OSStatus status = noErr;
	std::vector<UInt32> result;
	Boolean input = (isInput ? TRUE : FALSE);
	UInt32 size = 0;

	status = AudioDeviceGetPropertyInfo(id, 0, input, kAudioDevicePropertyDataSources, &size, NULL);
	if (status) {
		LOG_ERROR("Can't get device property info: kAudioDevicePropertyDataSources");
		return result;
	}

	if (!size) {
		return result;
	}

	UInt32 * ids = (UInt32 *) malloc(size);
	status = AudioDeviceGetProperty(id, 0, input, kAudioDevicePropertyDataSources, &size, ids);
	if (status) {
		LOG_ERROR("Can't get device property: kAudioDevicePropertyDataSources");
	} else {
		for (unsigned i = 0; i < (size / sizeof(UInt32)); i++) {
			result.push_back(ids[i]);
		}
	}

	free(ids);

	return result;
}

std::string CoreAudioUtilities::dataSourceName(AudioDeviceID id, bool isInput, UInt32 dsId) {
	UInt32 myDsId = dsId;
	CFStringRef name;
	AudioValueTranslation theTranslation = { &myDsId, sizeof(UInt32), &name, sizeof(CFStringRef) };
	UInt32 size = sizeof(AudioValueTranslation);
	std::string result;

	OSStatus status = AudioDeviceGetProperty(id, 0, (isInput ? TRUE : FALSE),
		kAudioDevicePropertyDataSourceNameForIDCFString, &size, &theTranslation);
	if (status) {
		LOG_ERROR("Can't get device property: kAudioDevicePropertyDataSourceNameForIDCFString");
	} else {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		CFStringGetCString(name, buffer, sizeof(buffer), kCFStringEncodingUTF8);
		result = std::string(buffer);
	}

	CFRelease(name);

	return result;
}
