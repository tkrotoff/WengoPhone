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

#include <sound/VolumeControl.h>

#include "MacVolumeControl.h"
#include "CoreAudioUtilities.h"

#include <sound/AudioDevice.h>

#include <util/Logger.h>

VolumeControl::VolumeControl(const std::string & deviceName, DeviceType deviceType)
	throw (NoSoundCardException, SoundMixerException) {

	if (!deviceName.empty()) {
		LOG_DEBUG("audio device=" + deviceName);
	}

	_volumeControlPrivate = NULL;
	std::map<AudioDeviceID, std::string> deviceMap;

	if (deviceType == DeviceTypeOutput) {
		// Looking for output device
		deviceMap = CoreAudioUtilities::audioDeviceMap(false);
		for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
			it != deviceMap.end();
			++it) {
			if ((*it).second == deviceName) {
				_volumeControlPrivate = new MacVolumeControl((*it).first, false);
				break;
			}
		}
		////
	} else if (deviceType == DeviceTypeInput) {
		// Looking for input device
		deviceMap = CoreAudioUtilities::audioDeviceMap(true);
		for (std::map<AudioDeviceID, std::string>::const_iterator it = deviceMap.begin();
			it != deviceMap.end();
			++it) {
			if ((*it).second == deviceName) {
				_volumeControlPrivate = new MacVolumeControl((*it).first, true);
				break;
			}
		}
		////
	} else {
		LOG_FATAL("unknown device type=" + String::fromNumber(deviceType));
	}
}

bool VolumeControl::setLevel(unsigned level) {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->setLevel(level);
	}
	return false;
}

int VolumeControl::getLevel() {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->getLevel();
	}
	return -1;
}

bool VolumeControl::setMute(bool mute) {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->setMute(mute);
	}
	return false;
}

bool VolumeControl::isMuted() {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->isMuted();
	}
	return false;
}

bool VolumeControl::close() {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->close();
	}
	return false;
}
