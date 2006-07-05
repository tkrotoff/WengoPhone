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

#if defined(OS_WINDOWS)
#include "win32/WinAudioDevice.h"
#elif defined(OS_MACOSX)
#include "mac/MacAudioDevice.h"
#elif defined(OS_LINUX)
#include "portaudio/PortAudioAudioDevice.h"
#endif // OS_LINUX

AudioDevice::AudioDevice() {
	_audioDevicePrivate = NULL;
}

AudioDevice::AudioDevice(const std::string & deviceId) {
#if defined(OS_WINDOWS)
	_audioDevicePrivate = new WinAudioDevice(deviceId);
#elif defined(OS_MACOSX)
	_audioDevicePrivate = new MacAudioDevice(deviceId);
#elif defined(OS_LINUX)
	_audioDevicePrivate = new PortAudioAudioDevice(deviceId);
#else
	_audioDevicePrivate = NULL;
#endif

}

AudioDevice::AudioDevice(const AudioDevice & audioDevice) {
	if (audioDevice.getAudioDevicePrivate()) {
#if defined(OS_WINDOWS)
		_audioDevicePrivate = new WinAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#elif defined(OS_MACOSX)
		_audioDevicePrivate = new MacAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#elif defined(OS_LINUX)
		_audioDevicePrivate = new PortAudioAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#else
		_audioDevicePrivate = NULL;
#endif
	} else {
		_audioDevicePrivate = NULL;
	}
}

AudioDevice & AudioDevice::operator = (const AudioDevice & audioDevice) {
	if (_audioDevicePrivate) {
		delete _audioDevicePrivate;
	}

	if (audioDevice.getAudioDevicePrivate()) {
#if defined(OS_WINDOWS)
		_audioDevicePrivate = new WinAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#elif defined(OS_MACOSX)
		_audioDevicePrivate = new MacAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#elif defined(OS_LINUX)
		_audioDevicePrivate = new PortAudioAudioDevice(audioDevice.getAudioDevicePrivate()->getId());
#else
		_audioDevicePrivate = NULL;
#endif
	} else {
		_audioDevicePrivate = NULL;
	}

	return *this;
}

AudioDevice::~AudioDevice() {
	if (_audioDevicePrivate) {
		delete _audioDevicePrivate;
	}
}

std::string AudioDevice::getName() const {
	std::string result;

	if (_audioDevicePrivate) {
		result = _audioDevicePrivate->getName();
	}

	return result;
}

std::string AudioDevice::getId() const {
	std::string result;

	if (_audioDevicePrivate) {
		result = _audioDevicePrivate->getId();
	}

	return result;
}
