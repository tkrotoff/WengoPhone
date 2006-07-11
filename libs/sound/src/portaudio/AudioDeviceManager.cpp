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

#include <sound/AudioDeviceManager.h>

#include "PAAudioDevice.h"

#include <portaudio.h>

#include <util/String.h>

std::list<AudioDevice> AudioDeviceManager::getInputDeviceList() {

	int i, numDevices;
	const PaDeviceInfo * deviceInfo;
	PaError err;
	std::list<AudioDevice> deviceList;

	Pa_Initialize();

	// Retrieve number of device found.
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		err = numDevices;
		Pa_Terminate();
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		return deviceList;
	}

	//iterate over devices
	for (i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);

		//if the device has input capability
		if (deviceInfo->maxInputChannels > 0) {

			std::string deviceName;
			if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice) {
				deviceName = "(default) ";
			} else {
				deviceName = "";
			}
			deviceName += deviceInfo->name;

			PAAudioDevice paAudioDevice(i, deviceName);
			AudioDevice audioDevice(paAudioDevice.getData());
			deviceList.push_back(audioDevice);
		}
	}

	Pa_Terminate();
	return deviceList;
}

std::list<AudioDevice> AudioDeviceManager::getOutputDeviceList() {
	int i, numDevices;
	const PaDeviceInfo * deviceInfo;
	PaError err;
	std::list<AudioDevice> deviceList;

	Pa_Initialize();

	// Retrieve number of device found.
	numDevices = Pa_GetDeviceCount();
	if (numDevices < 0) {
		err = numDevices;
		Pa_Terminate();
		fprintf(stderr, "An error occured while using the portaudio stream\n");
		fprintf(stderr, "Error number: %d\n", err);
		fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
		return deviceList;
	}

	//iterate over devices
	for (i = 0; i < numDevices; i++) {
		deviceInfo = Pa_GetDeviceInfo(i);

		//if the device has input capability
		if (deviceInfo->maxOutputChannels > 0) {

			std::string deviceName;
			if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice) {
				deviceName = "(default) ";
			} else {
				deviceName = "";
			}
			deviceName += deviceInfo->name;
			
			PAAudioDevice paAudioDevice(i, deviceName);
			AudioDevice audioDevice(paAudioDevice.getData());
			deviceList.push_back(audioDevice);
		}
	}

	Pa_Terminate();
	return deviceList;
}

AudioDevice AudioDeviceManager::getDefaultOutputDevice() {
	std::list<AudioDevice> list = getOutputDeviceList();

	std::list<AudioDevice>::iterator it;
	for(it = list.begin(); it != list.end(); it++) {
		String deviceName = (*it).getName();
		if(deviceName.contains("default")) {
			return (*it);
		}
	}

	return AudioDevice();
}

bool AudioDeviceManager::setDefaultOutputDevice(const AudioDevice & audioDevice) {
	return false;
}

AudioDevice AudioDeviceManager::getDefaultInputDevice() {
	std::list<AudioDevice> list = getInputDeviceList();

	std::list<AudioDevice>::iterator it;
	for(it = list.begin(); it != list.end(); it++) {
		String deviceName = (*it).getName();
		if(deviceName.contains("default")) {
			return (*it);
		}
	}

	return AudioDevice();
}

bool AudioDeviceManager::setDefaultInputDevice(const AudioDevice & audioDevice) {
	return false;
}

