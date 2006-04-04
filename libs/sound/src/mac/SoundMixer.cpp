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

#include "CoreAudioUtilities.h"

#include <iostream>

using namespace std;

SoundMixer::SoundMixer(const std::string & inputDeviceName, const std::string & outputDeviceName)
	throw (NoSoundCardException, SoundMixerException) {

	_outputVolume = NULL;
	_inputVolume = NULL;

	// Looking for input device
	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(true);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		if ((*it).second == inputDeviceName) {
			Volume
			_inputVolume = new Volume((*it).first, true);
			break;
		}
	}
	////

	// Looking for output device
	std::map<AudioDeviceID, std::string> deviceMap = CoreAudioUtilities::audioDeviceMap(false);
	for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
		it != deviceMap.end();
		++it) {
		if ((*it).second == outputDeviceName) {
			Volume
			_outputVolume = new Volume((*it).first, false);
			break;
		}
	}
	////

	if (!_outputVolume || !_inputVolume) {
		throw NoSoundCardException();
	}
}

void SoundMixer::closeMixers() {
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
