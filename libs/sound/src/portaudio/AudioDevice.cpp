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

#include <sound/AudioDevice.h>

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include "../win32/Win32VolumeControl.h"
#endif

#include <util/StringList.h>
#include <util/Logger.h>

#include <portaudio.h>

#include <cstdio>

/**
 * Retrieves the number of audio device found.
 */
int getNbDevices() {
	int nbDevices = Pa_GetDeviceCount();
	if (nbDevices < 0) {
		LOG_ERROR("Pa_GetDeviceCount()=" + String::fromNumber(nbDevices));
		PaError err = nbDevices;
		Pa_Terminate();
		LOG_ERROR("an error occured while using the portaudio stream, error message=" + String(Pa_GetErrorText(err)));
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
			if ((Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paALSA) ||
				(Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paOSS) ||
				(i != Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice)) {

				LOG_DEBUG("input device found=" + String(deviceInfo->name)
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
			if ((Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paALSA) ||
				(Pa_GetHostApiInfo(deviceInfo->hostApi)->type == paOSS) ||
				(i != Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice)) {

				LOG_DEBUG("output device found=" + String(deviceInfo->name)
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

	//Iterates over devices
	for (int i = 0; i < nbDevices; i++) {
		const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);
		if (i == Pa_GetDefaultOutputDevice()) {
			std::string name(deviceInfo->name);

			LOG_DEBUG("default output device=" + name);

			Pa_Terminate();
			return name;
		}
	}

	Pa_Terminate();

	return String::null;
}

std::string AudioDevice::getDefaultRecordDevice() {
	Pa_Initialize();

	int nbDevices = getNbDevices();
	if (nbDevices < 0) {
		return String::null;
	}

	//Iterates over devices
	for (int i = 0; i < nbDevices; i++) {
		const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);
		if (i == Pa_GetDefaultInputDevice()) {
			std::string name(deviceInfo->name);

			LOG_DEBUG("default input device=" + name);

			Pa_Terminate();
			return name;
		}
	}

	Pa_Terminate();

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
			const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);

			if (deviceInfo->maxOutputChannels > 0 && deviceInfo->name == deviceName) {
				int deviceId = Pa_HostApiDeviceIndexToDeviceIndex(deviceInfo->hostApi, i);

				LOG_DEBUG("found output device id=" + String::fromNumber(i) + " for="
					+ deviceName + " (" + String(deviceInfo->name) + ")");

				Pa_Terminate();

				//return deviceId;
				return i;
			}
		}
	}

	LOG_ERROR("no output device id found for=" + deviceName);

	Pa_Terminate();

	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	Pa_Initialize();

	if (deviceName.length()) {
		for (int i = 0; i < Pa_GetDeviceCount(); i++) {
			const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(i);

			if (deviceInfo->maxInputChannels > 0 && deviceInfo->name == deviceName) {
				int deviceId = Pa_HostApiDeviceIndexToDeviceIndex(deviceInfo->hostApi, i);

				LOG_DEBUG("found input device id=" + String::fromNumber(i) + " for="
					+ deviceName + " (" + String(deviceInfo->name) + ")");

				Pa_Terminate();

				//return deviceId;
				return i;
			}
		}
	}

	LOG_ERROR("no input device id found for=" + deviceName);

	Pa_Terminate();

	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & mixerName) {
#ifdef OS_WINDOWS
	unsigned nbMixers = ::mixerGetNumDevs();
	if (nbMixers == 0) {
		//No audio mixer device are present
		return -1;
	}

	MIXERCAPSA mixcaps;

	for (unsigned mixerId = 0; mixerId < nbMixers; mixerId++) {
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

bool AudioDevice::selectAsRecordDevice(const std::string & deviceName, TypeInput typeInput) {
#ifdef OS_WINDOWS
	int deviceId = getMixerDeviceId(deviceName);
	Win32VolumeControl * volumeControl = NULL;
	if (typeInput == TypeInputMicrophone) {
		try {
			volumeControl = new Win32VolumeControl(deviceId, Win32VolumeControl::Win32DeviceTypeWaveIn);
		} catch (const SoundMixerException &) {
			volumeControl = NULL;
			try {
				volumeControl = new Win32VolumeControl(deviceId, Win32VolumeControl::Win32DeviceTypeMicrophoneIn);
			} catch (const SoundMixerException &) {
				volumeControl = NULL;
			}
		}
	}
	return volumeControl->selectAsRecordDevice();
#endif

	return false;
}

AudioDevice::TypeInput AudioDevice::getSelectedRecordDevice(const std::string & deviceName) {
#ifdef OS_WINDOWS
#endif
	return TypeInputError;
}
