/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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
#include <util/SafeDelete.h>

#if defined(OS_WINDOWS)
	#include "win32/Win32VolumeControl.h"
#elif defined(OS_MACOSX)
	#include "mac/MacVolumeControl.h"
#elif defined(OS_LINUX)
	#ifdef OWSOUND_PORTAUDIO_SUPPORT
		#include "unix/UnixVolumeControl.h"
	#else
		#include "linux/LinuxVolumeControl.h"
	#endif
#else
	#error This OS has not been tested
#endif

VolumeControl::VolumeControl(const AudioDevice & audioDevice) {
	_mutex = new QMutex(QMutex::Recursive);

#if defined(OS_WINDOWS)
	_iVolumeControlPrivate = new Win32VolumeControl(audioDevice);
#elif defined(OS_MACOSX)
	_iVolumeControlPrivate = new MacVolumeControl(audioDevice);
#elif defined(OS_LINUX)
	#ifdef OWSOUND_PORTAUDIO_SUPPORT
		_iVolumeControlPrivate = new UnixVolumeControl(audioDevice);
	#else
		_iVolumeControlPrivate = new LinuxVolumeControl(audioDevice);
	#endif
#endif
}

VolumeControl::~VolumeControl() {
	OWSAFE_DELETE(_iVolumeControlPrivate);
	OWSAFE_DELETE(_mutex);
}

bool VolumeControl::setLevel(unsigned level) {
	QMutexLocker lock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setLevel(level);
	} else {
		return false;
	}
}

int VolumeControl::getLevel() {
	QMutexLocker lock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->getLevel();
	} else {
		return 0;
	}
}

bool VolumeControl::setMute(bool mute) {
	QMutexLocker lock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->setMute(mute);
	} else {
		return false;
	}
}

bool VolumeControl::isMuted() {
	QMutexLocker lock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isMuted();
	} else {
		return true;
	}
}

bool VolumeControl::isSettable() const {
	QMutexLocker lock(_mutex);

	if (_iVolumeControlPrivate) {
		return _iVolumeControlPrivate->isSettable();
	} else {
		return false;
	}
}
