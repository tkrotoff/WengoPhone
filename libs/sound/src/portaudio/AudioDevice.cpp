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

#include <StringList.h>
#include <Logger.h>

#include <global.h>

#include <portaudio.h>

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

#include <stdio.h>

/**
 * Retrieves the number of audio device found.
 */
int getNbDevices() {
	int nbDevices = Pa_GetDeviceCount();
	if (nbDevices < 0) {
		LOG_ERROR_C("Pa_GetDeviceCount()=" + String::fromNumber(nbDevices));
		PaError err = nbDevices;
		Pa_Terminate();
		LOG_ERROR_C("an error occured while using the portaudio stream, error message=" + String(Pa_GetErrorText(err)));
	}
	return nbDevices;
}

std::list<std::string> AudioDevice::getInputMixerDeviceList() {
	StringList deviceList;

	Pa_Initialize();

	int nbDevices = getNbDevices();
	if (nbDevices < 0) {
		return deviceList;
	}

	//Iterates over devices
	for (int i = 0; i < nbDevices; i++) {
		const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);

		if (deviceInfo->maxInputChannels > 0) {
			String deviceName;
			if ((Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paALSA) ||
				(Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paOSS) ||
				(i != Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice)) {

				LOG_DEBUG_C("input device found=" + String(deviceInfo->name)
					+ " " + String(Pa_GetHostApiInfo(deviceInfo->hostApi)->name));

				deviceList += deviceInfo->name;
			}
		}
	}

	Pa_Terminate();

	return deviceList;
}

std::list<std::string> AudioDevice::getOutputMixerDeviceList() {
	StringList deviceList;

	Pa_Initialize();

	int nbDevices = getNbDevices();
	if (nbDevices < 0) {
		return deviceList;
	}

	//Iterates over devices
	for (int i = 0; i < nbDevices; i++) {
		const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);

		if (deviceInfo->maxOutputChannels > 0) {
			String deviceName;
			if ((Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paALSA) ||
				(Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paOSS) ||
				(i != Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice)) {

				LOG_DEBUG_C("output device found=" + String(deviceInfo->name)
					+ " " + String(Pa_GetHostApiInfo(deviceInfo->hostApi)->name));

				deviceList += deviceInfo->name;
			}
		}
	}

	Pa_Terminate();

	return deviceList;
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	Pa_Initialize();

	int nbDevices = getNbDevices();
	if (nbDevices < 0) {
		return String::null;
	}

	//iterate over devices
	for (int i = 0; i < nbDevices; i++) {
		const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);
		if (i == Pa_GetDefaultOutputDevice()) {
			std::string name(deviceInfo->name);

			LOG_DEBUG_C("default device=" + name);

			Pa_Terminate();
			return name;
		}
	}

	Pa_Terminate();

	return String::null;
}

std::string AudioDevice::getDefaultRecordDevice() {
	return String::null;
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & /*deviceName*/) {
	return false;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & /*deviceName*/) {
	return false;
}

int AudioDevice::getWaveOutDeviceId(const std::string & deviceName) {
	Pa_Initialize();

	if (deviceName.length()) {
		for (int i = 0; i < Pa_GetDeviceCount(); i++) {
			const PaDeviceInfo * pdi = Pa_GetDeviceInfo(i);

			if (pdi->maxOutputChannels > 0 && pdi->name == deviceName) {
				int deviceId = Pa_HostApiDeviceIndexToDeviceIndex(pdi->hostApi, i);

				LOG_DEBUG_C("found output device id=" + String::fromNumber(i) + " for="
					+ deviceName + " (" + String(pdi->name) + ")");

				Pa_Terminate();

				//return deviceId;
				return i;
			}
		}
	}

	LOG_ERROR_C("no output device id found for=" + deviceName);

	Pa_Terminate();

	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	Pa_Initialize();

	if (deviceName.length()) {
		for (int i = 0; i < Pa_GetDeviceCount(); i++) {
			const PaDeviceInfo * pdi = Pa_GetDeviceInfo(i);

			if (pdi->maxInputChannels > 0 && pdi->name == deviceName) {
				int deviceId = Pa_HostApiDeviceIndexToDeviceIndex(pdi->hostApi, i);

				LOG_DEBUG_C("found input device id=" + String::fromNumber(i) + " for="
					+ deviceName + " (" + String(pdi->name) + ")");

				Pa_Terminate();

				//return deviceId;
				return i;
			}
		}
	}

	LOG_ERROR_C("no input device id found for=" + deviceName);

	Pa_Terminate();

	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & mixerName) {
#ifdef WIN32
	unsigned int nbMixers = ::mixerGetNumDevs();
	if (nbMixers == 0) {
		//No audio mixer device are present
		return -1;
	}

	MIXERCAPSA mixcaps;

	for (unsigned int mixerId = 0; mixerId < nbMixers; mixerId++) {
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsA(mixerId, & mixcaps, sizeof(MIXERCAPSA))) {
			if (mixerName == mixcaps.szPname) {
				return mixerId;
			}
		}
	}
#endif
	//Default deviceId is 0
	return 0;
}
