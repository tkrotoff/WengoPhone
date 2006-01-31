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

#ifndef PLAYSOUNDFILE_H
#define PLAYSOUNDFILE_H

#include <NonCopyable.h>

#include <string>

/**
 * Plays a sound file on a specific audio device.
 *
 * @author Tanguy Krotoff
 */
class PlaySoundFile : NonCopyable {
public:

	PlaySoundFile();

	~PlaySoundFile();

	/**
	 * Opens the wave out device given a name.
	 *
	 * If the device audio cannot be opened, opens the default one.
	 *
	 * @param deviceName name of the wave out device
	 */
	void setWaveOutDevice(const std::string & deviceName) {
		_audioDeviceName = deviceName;
	}

	/**
	 * Plays a wave sound file given its name.
	 *
	 * @param filename name of the wave file to play
	 * @return true if the sound file was played correctly; false otherwise
	 */
	bool play(const std::string & filename);

private:

	std::string _audioDeviceName;
};

#endif	//PLAYSOUNDFILE_H
