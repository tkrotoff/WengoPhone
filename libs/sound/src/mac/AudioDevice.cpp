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
		result.push_back((*it).second);
	}

	return result;
}

std::list<std::string> AudioDevice::getOutputMixerDeviceList() {
	std::list<std::string> result;

	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(false);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		result.push_back((*it).second);
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
		return result;
	}

	result = CoreAudioUtilities::audioDeviceName(device, false);

	return result;
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & /*deviceName*/) {
	return false;
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

	return result;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & /*deviceName*/) {
	return false;
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
