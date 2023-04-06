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

#ifndef OWVOLUMECONTROL_H
#define OWVOLUMECONTROL_H

#include <sound/IVolumeControl.h>
#include <sound/sounddll.h>

#include <QtCore/QMutex>

class AudioDevice;

/**
 * Volume control implementation.
 *
 * @see IVolumeControl
 */
class VolumeControl :  public IVolumeControl {
public:

	/**
	 * @param audioDevice the AudioDevice we want to set the volume
	 * It it copied internally.
	 */
	SOUND_API VolumeControl(const AudioDevice & audioDevice);

	SOUND_API ~VolumeControl();

	SOUND_API bool setLevel(unsigned level);

	SOUND_API int getLevel();

	SOUND_API bool setMute(bool mute);

	SOUND_API bool isMuted();

	SOUND_API bool isSettable() const;

private:

	/** System dependant implementation. */
	IVolumeControl * _iVolumeControlPrivate;

	mutable QMutex *_mutex;
};

#endif	//OWVOLUMECONTROL_H
