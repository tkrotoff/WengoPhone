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

#ifndef OWVOLUMECONTROL_H
#define OWVOLUMECONTROL_H

#include <sound/AudioDevice.h>
#include <sound/IVolumeControl.h>

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
	VolumeControl(const AudioDevice & audioDevice);

	bool setLevel(unsigned level);

	int getLevel();

	bool setMute(bool mute);

	bool isMuted();

	bool isSettable() const;

private:

	/** System dependant implementation. */
	IVolumeControl * _iVolumeControlPrivate;
};

#endif	//OWVOLUMECONTROL_H
