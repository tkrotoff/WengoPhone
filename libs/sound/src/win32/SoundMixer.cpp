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

#include <sound/SoundMixer.h>

#include <sound/AudioDevice.h>

#include <util/Logger.h>

#include "Volume.h"

#include <string>
#include <iostream>
using namespace std;

SoundMixer::SoundMixer(const std::string & inputDeviceName, const std::string & outputDeviceName)
		throw (NoSoundCardException, SoundMixerException) {

	int inputDeviceId = AudioDevice::getMixerDeviceId(inputDeviceName);
	int outputDeviceId = AudioDevice::getMixerDeviceId(outputDeviceName);

	if (!inputDeviceName.empty() && !outputDeviceName.empty()) {
		LOG_DEBUG("SoundMixer: audio devices: in= " + inputDeviceName + String::fromNumber(inputDeviceId)
						+ " out= " + outputDeviceName + String::fromNumber(outputDeviceId));
	}

	if (inputDeviceId == -1 || outputDeviceId == -1) {
		throw NoSoundCardException();
	}

	//Volume throws an exception inside the constructor
	//so _outputVolume can be NULL
	//Check SoundMixer::closeMixers()
	_outputVolume = NULL;
	try {
		_outputVolume = new Volume(outputDeviceId, Volume::MasterVolume);
	} catch (const SoundMixerException &) {
		_outputVolume = NULL;
	}

	_inputVolume = NULL;
	try {
		_inputVolume = new Volume(inputDeviceId, Volume::WaveIn);
	} catch (const SoundMixerException &) {
		try {
			_inputVolume = new Volume(inputDeviceId, Volume::MicrophoneIn);
		} catch (const SoundMixerException &) {
			_inputVolume = NULL;
		}
	}
}

void SoundMixer::closeMixers() {
	if (_inputVolume) {
		_inputVolume->closeMixer();
	}
	if (_outputVolume) {
		_outputVolume->closeMixer();
	}
}

int SoundMixer::getOutputVolume() {
	if (_outputVolume) {
		return _outputVolume->getVolume();
	}
	return 0;
}

int SoundMixer::getInputVolume() {
	if (_inputVolume) {
		return _inputVolume->getVolume();
	}
	return 0;
}

void SoundMixer::setOutputVolume(int volume) {
	if (_outputVolume) {
		_outputVolume->setVolume(volume);
	}
}

void SoundMixer::setInputVolume(int volume) {
	if (_inputVolume) {
		_inputVolume->setVolume(volume);
	}
}
