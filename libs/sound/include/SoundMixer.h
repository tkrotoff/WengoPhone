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

#ifndef SOUNDMIXER_H
#define SOUNDMIXER_H

#include <SoundMixerException.h>
#include <NoSoundCardException.h>

#include <NonCopyable.h>

#include <string>

class Volume;

/**
 * Sound mixer managing: gets and sets the volume of
 * a microphone audio device plus a master/wave out audio device.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class SoundMixer : NonCopyable {
public:

	/**
	 * Constructs a SoundMixer object.
	 *
	 * @param inputDeviceName microphone audio device name
	 * @param outputDeviceName master or wave out audio device name
	 * @exception NoSoundCardException no sound card available on the system
	 * @exception SoundMixerException an error occured while manipulating an audio mixer
	 */
	SoundMixer(const std::string & inputDeviceName, const std::string & outputDeviceName)
		throw (NoSoundCardException, SoundMixerException);

	~SoundMixer() {
		closeMixers();
	}

	/**
	 * Sets the microphone volume.
	 *
	 * @param volume new input volume (0 to 100)
	 */
	void setInputVolume(int volume);

	/**
	 * Gets the input (microphone) volume.
	 *
	 * @return the input volume (0 to 100), -1 if an error occured
	 */
	int getInputVolume();

	/**
	 * Gets the output (master or wave out) volume.
	 *
	 * @return the output volume (0 to 100), -1 if an error occured
	 */
	int getOutputVolume();

	/**
	 * Sets the master or wave out volume.
	 *
	 * @param volume new output volume (0 to 100)
	 */
	void setOutputVolume(int volume);

	/**
	 * (Un)mute mic
	 *
	 * @param mute or not
	 */
	void setMicPlayBack(bool mute);

	/**
	 * mic is muted.
	 *
	 * @return true if mic is muted false otherwise
	 */
	bool isPlaybackMuted();

	/**
	 * Closes all the audio mixers.
	 */
	void closeMixers();

private:

	/**
	 * Master or wave out volume component.
	 */
	Volume * _outputVolume;

	/**
	 * Microphone volume component.
	 */
	Volume * _inputVolume;
};

#endif	//SOUNDMIXER_H
