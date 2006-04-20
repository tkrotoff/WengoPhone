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

#include "UnixVolumeControl.h"

#include <sound/AudioDevice.h>

#include <util/Logger.h>

VolumeControl::VolumeControl(const std::string & deviceName, DeviceType deviceType) throw (NoSoundCardException, SoundMixerException) {
	int deviceId = AudioDevice::getMixerDeviceId(deviceName);

	if (!deviceName.empty()) {
		LOG_DEBUG("audio device=" + deviceName + String::fromNumber(deviceId));
	}

	if (deviceId == -1) {
		throw NoSoundCardException(deviceName);
	}

	//Volume throws an exception inside the constructor so _volume can be NULL
	//Check VolumeControl::closeMixer()
	_volumeControlPrivate = NULL;

	//Output device
	if (deviceType == DeviceTypeOutput) {
		try {
			_volumeControlPrivate = new UnixVolumeControl(deviceId, UnixVolumeControl::UnixDeviceTypePcm);
		} catch (const SoundMixerException &) {
			_volumeControlPrivate = NULL;
		}
	}

	//Input device
	else if (deviceType == DeviceTypeInput) {
		try {
			_volumeControlPrivate = new UnixVolumeControl(deviceId, UnixVolumeControl::UnixDeviceTypeMic);
		} catch (const SoundMixerException &) {
			_volumeControlPrivate = NULL;
			try {
				_volumeControlPrivate = new UnixVolumeControl(deviceId, UnixVolumeControl::UnixDeviceTypeMic);
			} catch (const SoundMixerException &) {
				_volumeControlPrivate = NULL;
			}
		}
	}

	else {
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

bool VolumeControl::selectAsRecordDevice() {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->selectAsRecordDevice();
	}
	return false;
}

bool VolumeControl::close() {
	if (_volumeControlPrivate) {
		return _volumeControlPrivate->close();
	}
	return false;
}
