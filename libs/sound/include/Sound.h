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

#ifndef SOUND_H
#define SOUND_H

#include <NonCopyable.h>

#include <string>

/**
 * Reimplementation of QSound from Qt.
 *
 * The big difference is that you can choose the wave out audio device.
 * Currently it only plays wave audio files.
 *
 * Under Unix it uses QSound from the Qt library so the wave out audio device
 * is not handled.
 *
 * @author Tanguy Krotoff
 */
class Sound : NonCopyable {
public:

	/**
	 * Constructs a Sound that can quickly play the sound in a file named filename.
	 *
	 * @param filename sound file
	 */
	Sound(const std::string & filename);

	~Sound();

	/**
	 * Sets the sound to repeat loops times when it is played.
	 * Passing the value -1 will cause the sound to loop indefinitely.
	 *
	 * @param loops number of time the sound has to be played, -1 for infinite
	 */
	void setLoops(int loops);

	/**
	 * Sets the wave out audio device given its name.
	 *
	 * @param deviceName wave out audio device name
	 * @return true if the device was changed, false otherwise (not implemented yet)
	 */
	bool setWaveOutDevice(const std::string & deviceName);

	/**
	 * Plays the sound in a file called filename.
	 *
	 * @param filename sound file
	 * @param deviceName wave out audio device name
	 */
	static void play(const std::string & filename, const std::string & deviceName = NULL);

	/**
	 * Plays the sound.
	 */
	void play();

	/**
	 * Stops playing the sound.
	 */
	void stop();

private:

	class SoundPrivate;

	/**
	 * System-dependant implementation.
	 */
	SoundPrivate * _soundPrivate;
};

#endif	//SOUND_H
