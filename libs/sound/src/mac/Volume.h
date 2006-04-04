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

#ifndef VOLUME_H
#define VOLUME_H

#include <CoreAudio/CoreAudio.h>

/**
 * Get and change the volume of a MacOS X audio device.
 *
 * @author Philippe Bernery
 */
class Volume {

public:

	Volume(AudioDeviceID deviceId, bool isInput);

	/**
	 * Gets the current volume of the audio device.
	 *
	 * On MacOS X, the volume of the output can't be gotten.
	 *
	 * @return volume of the audio mixer, or -1 if it failed
	 */
	int getVolume();

	/**
	 * Sets the current volume of the audio device.
	 *
	 * @param volume sound volume of the audio mixer
	 * @return true if the volume has been changed, false otherwise
	 */
	bool setVolume(unsigned volume);

private:

	/** ID of the device. */
	AudioDeviceID _deviceId;

	/** Channel to use (input or output) */
	Boolean _isInput;

};

#endif	//VOLUME_H
