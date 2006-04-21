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

#ifndef MACVOLUMECONTROL_H
#define MACVOLUMECONTROL_H

#include <sound/IVolumeControl.h>

#include <CoreAudio/CoreAudio.h>

/**
 * Gets and change the volume of a MacOS X audio device.
 *
 * @author Philippe Bernery
 */
class MacVolumeControl : public IVolumeControl {
public:

	MacVolumeControl(AudioDeviceID deviceId, bool isInput);

	bool setLevel(unsigned level);

	int getLevel();

	bool setMute(bool mute);

	bool isMuted();

	bool close();

private:

	/** ID of the device. */
	AudioDeviceID _deviceId;

	/** Channel to use (input or output) */
	Boolean _isInput;
};

#endif	//MACVOLUMECONTROL_H
