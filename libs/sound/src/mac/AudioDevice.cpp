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

#include <sound/AudioDevice.h>

#include "CoreAudioUtilities.h"

#include <util/Logger.h>

std::list<std::string> AudioDevice::getInputMixerDeviceList() {
	std::list<std::string> result;

	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(true);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		std::vector<UInt32> list = CoreAudioUtilities::dataSourcesList((*it).first, true);
		for (std::vector<UInt32>::const_iterator dsit = list.begin();
			dsit != list.end();
			++dsit) {
			result.push_back((*it).second + " - " + CoreAudioUtilities::dataSourceName((*it).first, true, (*dsit)));
		}
	}

	return result;
}

std::list<std::string> AudioDevice::getOutputMixerDeviceList() {
	std::list<std::string> result;

	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(false);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		std::vector<UInt32> list = CoreAudioUtilities::dataSourcesList((*it).first, false);
		for (std::vector<UInt32>::const_iterator dsit = list.begin();
			dsit != list.end();
			++dsit) {
			result.push_back((*it).second + " - " + CoreAudioUtilities::dataSourceName((*it).first, false, (*dsit)));
		}
	}

	return result;
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	OSStatus status = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	std::string result;

	status = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &size, &device);
	if (status) {
		LOG_ERROR("can't get default output device");
		return String::null;
	}

	result = CoreAudioUtilities::audioDeviceName(device, false);

	UInt32 dataSourceId;
	size = sizeof(UInt32);
	status = AudioDeviceGetProperty(device, 0, 0, kAudioDevicePropertyDataSource, &size, &dataSourceId);
	if (status) {
		LOG_ERROR("can't get default output data source");
		return String::null;
	}

	result += " - " + CoreAudioUtilities::dataSourceName(device, false, dataSourceId);

	return result;
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & deviceName) {
	OSStatus status = noErr;
	AudioDeviceID device;
	bool found = false;

	// Looking the device ID regarding the name
	std::map<AudioDeviceID, std::string> map = CoreAudioUtilities::audioDeviceMap(false);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = map.begin();
		it != map.end();
		++it) {
		if ((*it).second == deviceName) {
			device = (*it).first;
			found = true;
			break;
		}
	}

	if (!found) {
		LOG_ERROR("no device named " + deviceName + " found");
		return false;
	}
	////

	UInt32 size = sizeof(AudioDeviceID);
	status = AudioHardwareSetProperty(kAudioHardwarePropertyDefaultOutputDevice, size, &device);
	if (status) {
		LOG_ERROR("can't set default output device to " + String::fromNumber(device));
		return false;
	}

	return true;
}

std::string AudioDevice::getDefaultRecordDevice() {
	OSStatus status = noErr;
	AudioDeviceID device;
	UInt32 size = sizeof(AudioDeviceID);
	std::string result;

	status = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice, &size, &device);
	if (status) {
		LOG_ERROR("can't get default input device");
		return result;
	}

	result = CoreAudioUtilities::audioDeviceName(device, true);

	UInt32 dataSourceId;
	size = sizeof(UInt32);
	status = AudioDeviceGetProperty(device, 0, 1, kAudioDevicePropertyDataSource, &size, &dataSourceId);
	if (status) {
		LOG_ERROR("can't get default input data source");
		return String::null;
	}

	result += " - " + CoreAudioUtilities::dataSourceName(device, false, dataSourceId);

	return result;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & deviceName) {
	OSStatus status = noErr;
	AudioDeviceID device;
	bool found = false;

	// Looking the device ID regarding the name
	std::map<AudioDeviceID, std::string> map = CoreAudioUtilities::audioDeviceMap(true);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = map.begin();
		it != map.end();
		++it) {
		if ((*it).second == deviceName) {
			device = (*it).first;
			found = true;
			break;
		}
	}

	if (found) {
		LOG_ERROR("no device named " + deviceName + " found");
		return false;
	}
	////

	UInt32 size = sizeof(AudioDeviceID);
	status = AudioHardwareSetProperty(kAudioHardwarePropertyDefaultInputDevice, size, &device);
	if (status) {
		LOG_ERROR("can't set default output device to " + String::fromNumber(device));
		return false;
	}

	return true;
}

int AudioDevice::getWaveOutDeviceId(const std::string & deviceName) {
	int result = 0;

	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(false);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		if ((*it).second == deviceName) {
			result = (*it).first;
			break;
		}
	}

	return result;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	int result = 0;

	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(true);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		if ((*it).second == deviceName) {
			result = (*it).first;
			break;
		}
	}

	return result;
}

int AudioDevice::getMixerDeviceId(const std::string & mixerName) {
	return 0;
}

bool AudioDevice::selectAsRecordDevice(const std::string & deviceName, TypeInput typeInput) {
	return false;
}

AudioDevice::TypeInput AudioDevice::getSelectedRecordDevice(const std::string & deviceName) {
	return TypeInputError;
}
