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

#include <AudioDevice.h>

#include <util/StringList.h>

#include <portaudio.h>

#define PRINT_HOSTAPI

#include <stdio.h>

std::list < std::string > AudioDevice::getInputMixerDeviceList() {
	int i, numDevices;
	const PaDeviceInfo * deviceInfo;
	PaError err;
	StringList deviceList;

	Pa_Initialize();

	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		printf("ERROR: Pa_CountDevices returned 0x%x\n", numDevices);
		err = numDevices;
		Pa_Terminate();
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		return StringList();
	}

	//iterate over devices
	for (i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);

		if (deviceInfo->maxInputChannels > 0) {
			String deviceName;
			if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice) {
				deviceName = "(default) ";
			} else {
				deviceName = "";
			}
#ifdef PRINT_HOSTAPI
			deviceName += Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
			deviceName += ": ";
#endif
			deviceName += deviceInfo->name;
			deviceList += deviceName;
		}
	}
	return deviceList;
}

std::list < std::string > AudioDevice::getOutputMixerDeviceList() {
	int i, numDevices;
	const PaDeviceInfo * deviceInfo;
	PaError err;
	StringList deviceList;

	Pa_Initialize();

	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		printf("ERROR: Pa_CountDevices returned 0x%x\n", numDevices);
		err = numDevices;
		Pa_Terminate();
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		return StringList();
	}

	//iterate over devices
	for (i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);

		if (deviceInfo->maxOutputChannels > 0) {
			String deviceName;
			if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice) {
				deviceName = "(default) ";
			} else {
				deviceName = "";
			}
#ifdef PRINT_HOSTAPI
			deviceName += Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
			deviceName += ": ";
#endif
			deviceName += deviceInfo->name;
			deviceList += deviceName;
		}
	}
	return deviceList;
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	int i, numDevices;
	const PaDeviceInfo * deviceInfo;
	PaError err;

	Pa_Initialize();

	//retrive number of device found
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		printf("ERROR: Pa_CountDevices returned 0x%x\n", numDevices);
		err = numDevices;
		Pa_Terminate();
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		return "";
	}

	//iterate over devices
	for (i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);
		if (i == Pa_GetDefaultOutputDevice()) {
			String deviceName = "";
#ifdef PRINT_HOSTAPI
			deviceName += Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
			deviceName += ": ";
#endif
			deviceName += deviceInfo->name;
			return String(deviceName);
		}
	}
	return "";
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & /*deviceName*/) {
	return false;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & /*deviceName*/) {
	return false;
}

int AudioDevice::getWaveOutDeviceId(const std::string & deviceName) {
	StringList deviceList = getOutputMixerDeviceList();

	if (deviceName.length()) {
		for (unsigned int currentDeviceIndex = 0;
		currentDeviceIndex < deviceList.size();
		++currentDeviceIndex) {
			if (deviceName == deviceList.get(currentDeviceIndex)) {
				return currentDeviceIndex;
			}
		}
	}
	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	StringList deviceList = getInputMixerDeviceList();

	if (deviceName.length()) {
		for (unsigned int currentDeviceIndex = 0;
		currentDeviceIndex < deviceList.size();
		++currentDeviceIndex) {
			if (deviceName == deviceList.get(currentDeviceIndex)) {
				return currentDeviceIndex;
			}
		}
	}
	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & /*mixerName*/) {
	return 0;
}

std::string AudioDevice::getDefaultRecordDevice() {
	return "";
}
