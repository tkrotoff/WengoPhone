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

#include <sound/VolumeControl.h>

#include <cutil/global.h>

#if defined(OS_WINDOWS)
#include "win32/WinVolumeControl.h"
#elif defined(OS_MACOSX)
#include "mac/MacVolumeControl.h"
#elif defined(OS_LINUX)
#include "portaudio/PortAudioVolumeControl.h"
#endif // OS_LINUX

VolumeControl::VolumeControl() {
	_iVolumeControlPrivate = NULL;
}

VolumeControl::VolumeControl(AudioDevice audioDevice) {
	_audioDevice = audioDevice;

#if defined(OS_WINDOWS)
	_iVolumeControlPrivate = new WinVolumeControl(audioDevice);
#elif defined(OS_MACOSX)
	_iVolumeControlPrivate = new MacVolumeControl(audioDevice);
#elif defined(OS_LINUX)
	_iVolumeControlPrivate = new PortAudioVolumeControl(audioDevice);
#else
	_iVolumeControlPrivate = NULL;
#endif

}

bool VolumeControl::setLevel(unsigned level) {
	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setLevel(level);
	} else {
		return false;
	}
}

int VolumeControl::getLevel() const {
	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->getLevel();
	} else {
		return 0;
	}
}

bool VolumeControl::setMute(bool mute) {
	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setMute(mute);
	} else {
		return false;
	}
}

bool VolumeControl::isMuted() const {
	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isMuted();
	} else {
		return true;
	}
}

bool VolumeControl::isSettable() const {
	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isSettable();
	} else {
		return false;
	}
}
