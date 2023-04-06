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

#ifndef SOUNDTHREAD_H
#define SOUNDTHREAD_H

#include <thread/Thread.h>

#include <string>

/**
 * Plays a sound in a threaded way.
 *
 * Helper for the Sound class. 
 *
 * @see Sound
 * @see Thread
 * @author Tanguy Krotoff
 */
class SoundThread : public Thread {
public:

	/**
	 * Plays a sound file given its filename.
	 *
	 * @param filename sound file to play
	 */
	SoundThread(const std::string & filename);

	~SoundThread();

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
	 * Stops playing the sound.
	 */
	void stop();

protected:

	/**
	 * Starts the thread.
	 */
	virtual void run();

private:

	/**
	 * Sound filename.
	 */
	std::string _filename;

	/**
	 * Wave out audio device name.
	 */
	std::string _deviceName;

	/**
	 * Number of time the sound has to be played, -1 for infinite
	 */
	int _loops;

	/**
	 * Stops or continues playing the sound.
	 */
	bool _stop;
};

#endif	//SOUNDTHREAD_H
