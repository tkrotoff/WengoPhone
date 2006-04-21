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

#include "CoreAudioUtilities.h"

#include <util/Logger.h>

MacVolumeControl::MacVolumeControl(AudioDeviceID deviceId, bool isInput)
	: _deviceId(deviceId), _isInput((isInput ? TRUE : FALSE)) {
}

int MacVolumeControl::getLevel() {
	OSStatus status = noErr;
	UInt32 size = 0;
	Float32 level = 0.0;

	size = sizeof(level);
	status = AudioDeviceGetProperty(_deviceId, 0, _isInput, kAudioDevicePropertyVolumeScalar, &size, &level);
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

	size = sizeof(fVolume);
	status = AudioDeviceSetProperty(_deviceId, 0, 0, _isInput, kAudioDevicePropertyVolumeScalar, size, &fVolume);
	if (status) {
		LOG_ERROR("can't set device property: kAudioDevicePropertyVolumeScalar\n");
		return false;
	} else {
		return true;
	}
}

bool MacVolumeControl::setMute(bool mute) {
	return false;
}

bool MacVolumeControl::isMuted() {
	return false;
}

bool MacVolumeControl::close() {
	return false;
}
