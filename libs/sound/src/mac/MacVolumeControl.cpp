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

#include "MacVolumeControl.h"

#include "MacAudioDevice.h"

#include <util/Logger.h>

#include <CoreAudio/CoreAudio.h>

MacVolumeControl::MacVolumeControl(const AudioDevice & audioDevice)
	: VolumeControl() {
	_audioDevice = audioDevice;
}

int MacVolumeControl::getLevel() const {
	OSStatus status = noErr;
	UInt32 size = 0;
	Float32 level = 0.0;

	MacAudioDevice * macAudioDevice = dynamic_cast<MacAudioDevice *>(_audioDevice.getAudioDevicePrivate());

	size = sizeof(level);
	status = AudioDeviceGetProperty(macAudioDevice->getAudioDeviceID(), 0,
		macAudioDevice->isInput(), kAudioDevicePropertyVolumeScalar, &size, &level);
	if (status) {
		LOG_ERROR("can't get device property: kAudioDevicePropertyVolumeScalar\n");
		return -1;
	} else {
		return (int) (level * 100.0);
	}
}

bool MacVolumeControl::setLevel(unsigned level) {
	OSStatus status = noErr;
	UInt32 size = 0;
	Float32 fVolume = level / 100.0;

	MacAudioDevice * macAudioDevice = dynamic_cast<MacAudioDevice *>(_audioDevice.getAudioDevicePrivate());

	size = sizeof(fVolume);
	status = AudioDeviceSetProperty(macAudioDevice->getAudioDeviceID(), 0, 0,
		macAudioDevice->isInput(), kAudioDevicePropertyVolumeScalar, size, &fVolume);
	if (status) {
		LOG_ERROR("can't set device property: kAudioDevicePropertyVolumeScalar\n");
		return false;
	} else {
		return true;
	}
}

bool MacVolumeControl::setMute(bool mute) {
	//TODO
	return false;
}

bool MacVolumeControl::isMuted() const {
	//TODO
	return false;
}

bool MacVolumeControl::isSettable() const {
	//TODO
	return false;
}

